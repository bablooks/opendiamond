#
#  The OpenDiamond Platform for Interactive Search
#
#  Copyright (c) 2009-2011 Carnegie Mellon University
#  All rights reserved.
#
#  This software is distributed under the terms of the Eclipse Public
#  License, Version 1.0 which can be found in the file named LICENSE.
#  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
#  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#
#
# Functions to access the Mirage virtual machine repository
#

from wsgiref.util import shift_path_info
from urllib import unquote_plus
import subprocess
import os
import re
import struct
import fnmatch

__all__ = ['scope_app', 'object_app']
BASEURL = 'mirage'

MIRAGE_REPOSITORY = None

# this expression only matches files because the mode starts with '-'
MGLV_RE = r"""
    (?P<mode>-.{9})\s+
    (?P<nlink>\d+)\s+
    (?P<uid>%s)\s+
    (?P<gid>\d+)\s+
    (?P<size>\d+)\s
    (?P<mtime>.{16})\s
    (?P<sha1sum>[a-fA-F0-9]{40})\s
    \.(?P<path>%s)$"""


# Create file-like wrapper around objects in the Mirage content store
class MirageParseError(Exception):
    pass


class MirageObject(object):
    def __init__(self, sha1sum):
        # Map from sha1 to content store
        link = os.path.join(MIRAGE_REPOSITORY, "slhome",
                            "com.ibm.mirage.sha1id", sha1sum[:2], sha1sum)
        idx, offset, length = map(int, os.readlink(link).split())

        # Set up wrapper around the object in the content store
        path = os.path.join(MIRAGE_REPOSITORY, "contentfiles",
                            "contents.%d" % idx)
        self.contentfile = open(path, 'rb')
        self.contentfile.seek(offset)
        self.length = length

        # Parse out the header
        chunk = self.contentfile.read(27)
        if chunk[:23] != 'com.ibm.mirage.content\0':
            raise MirageParseError('header mismatch')
        length = struct.unpack('>I', chunk[23:])[0]

        chunk = self.contentfile.read(length + 8)
        if chunk[:length] != 'com.ibm.mirage.sha1id\0':
            raise MirageParseError('object identifier is not sha1id')
        length = struct.unpack('>Q', chunk[length:])[0]

        chunk = self.contentfile.read(length + 8)
        if chunk[:length-1] != sha1sum:
            raise MirageParseError('sha1 checksum mismatch')
        length = struct.unpack('>Q', chunk[length:])[0]
        if length != self.length:
            raise MirageParseError('length mismatch')

    def close(self):
        self.contentfile.close()

    def read(self, size=4096):
        if size > self.length:
            size = self.length
        chunk = self.contentfile.read(size)
        self.length = self.length - len(chunk)
        return chunk


def mirage_list_object_ids(image_id, paths, uidregex=r"\d+"):
    pathregex = '|'.join(fnmatch.translate(path)[:-1] for path in paths)
    mglv_re = re.compile(MGLV_RE % (uidregex, pathregex), re.X)

    p = subprocess.Popen(['sudo', 'mg', 'list-verbose', '-R',
                          "file://" + MIRAGE_REPOSITORY, image_id],
                         stdout=subprocess.PIPE, close_fds=True)
    for line in p.stdout:
        m = mglv_re.match(line)
        if not m:
            continue
        res = m.groupdict()
        if res['size'] == '0':
            continue
        yield res['path'], res['sha1sum']
    p.stdout.close()


def mirage_extract_etc_passwd(image_id):
    uidmap = {}
    for path, sha1sum in mirage_list_object_ids(
            image_id, ['/etc/passwd', '/etc/group']):
        if path == '/etc/passwd':
            file = MirageObject(sha1sum).read(1024*1024)
            for line in file.split('\n'):
                try:
                    # user, secret, uid, gid, name, dir, shell
                    user, _, uid, _, _, _, _ = line.split(':')
                    uidmap[user] = uid
                except ValueError:
                    pass
        if uidmap:
            break
    return uidmap


def mirage_list_verbose(image_id, paths, users=None):
    yield '<?xml version="1.0" encoding="UTF-8" ?>\n'
    yield '<objectlist>\n'
    try:
        if users:
            uidmap = mirage_extract_etc_passwd(image_id)
            uidregex = r'|'.join(str(uidmap[user]) for user in users)
        else:
            uidregex = r'\d+'

        for _, sha1sum in mirage_list_object_ids(image_id, paths, uidregex):
            yield '<count adjust="1"/><object src="obj/%s"/>\n' % sha1sum

    except KeyError:
        pass
    yield '</objectlist>'


def init(config):
    global MIRAGE_REPOSITORY  # pylint: disable=global-statement
    MIRAGE_REPOSITORY = config.mirage_repository


def scope_app(environ, start_response):
    root = shift_path_info(environ)
    if root == 'obj':
        return object_app(environ, start_response)

    querydict = {}
    qs = environ.get('QUERY_STRING', '').split('&')
    ql = [map(unquote_plus, comp.split('=', 1)) for comp in qs if '=' in comp]
    for k, v in ql:
        querydict.setdefault(k, []).append(v)

    image_id = 'com.ibm.mirage.sha1id/' + root.lower()

    start_response("200 OK", [('Content-Type', "text/xml")])
    return mirage_list_verbose(image_id,
                               querydict.get('path', ['*']),
                               querydict.get('user', None))


def object_app(environ, start_response):
    sha1sum = environ['PATH_INFO'][1:].lower()
    f = MirageObject(sha1sum)
    etag = '"' + sha1sum + '"'

    headers = [('Content-Length', str(f.length)), ('ETag', etag)]

    if_none = environ.get('HTTP_IF_NONE_MATCH')
    if if_none and (if_none == '*' or etag in if_none):
        start_response("304 Not Modified", headers)
        return [""]

    start_response("200 OK", headers)
    # wrap the file object in an iterator that reads the file in 64KB
    # blocks instead of line-by-line.
    return environ['wsgi.file_wrapper'](f, 65536)
