#
#  The OpenDiamond Platform for Interactive Search
#  Version 6
#
#  Copyright (c) 2009-2011 Carnegie Mellon University
#  All rights reserved.
#
#  This software is distributed under the terms of the Eclipse Public
#  License, Version 1.0 which can be found in the file named LICENSE.
#  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
#  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from datetime import datetime, timedelta
from PIL import Image
from threading import Lock
from wsgiref.util import shift_path_info
from urllib2 import urlopen
from pyramid import *
from cStringIO import StringIO

__all__ = ['scope_app', 'object_app']
baseurl = 'gigapan'

class GigaPanInfoCache(object):
    def __init__(self):
        self._cache = {}
        self._lock = Lock()
    def __getitem__(self, id):
        with self._lock:
            if id not in self._cache:
                self._cache[id] = stat_gigapan(id)
            return self._cache[id]

gigapan_info_cache = GigaPanInfoCache()

def scope_app(environ, start_response):
    root = shift_path_info(environ)
    if root == 'obj':
	return object_app(environ, start_response)

    gigapan_id = root.strip()

    start_response("200 OK", [('Content-Type', "text/xml")])
    return expand_urls(int(gigapan_id))
    

def expand_urls(id):
    yield '<?xml version="1.0" encoding="UTF-8" ?>\n'
    yield '<objectlist>\n'

    info = gigapan_info_cache[id]
    height = info.get('height')
    width = info.get('width')
    levels = info.get('levels')

    # still calculating stupidly
    iter = iter_coords(width, height)
    nPhotos = 0
    try:
        while True:
            coord = iter.next()
            nPhotos += 1
    except StopIteration:
        yield '<count adjust="%d"/>\n' % nPhotos

    iter = iter_coords(width, height)

    try:
        while True:
            coord = iter.next()
            yield '<object src="obj/%s/%s/%s/%s"/>\n' % (id, coord[0], 
                                                         coord[1], coord[2])
    except StopIteration:
        pass

    yield '</objectlist>'



def object_app(environ, start_response):
    components = environ['PATH_INFO'][1:].split('/')
    id = int(components[0])
    lvl = int(components[1])
    col = int(components[2])
    row = int(components[3])
    url_components = ["http://share.gigapan.org/gigapans0/", str(id),
                      '/tiles/', path_to_tile(lvl, col, row)]
    url = ''.join(url_components)

    obj = urlopen(url)
    info = gigapan_info_cache[id]
    real_level = (info.get('levels') - 1) - lvl
    level_width = info.get('width') / 2**real_level
    level_height = info.get('height') / 2**real_level
    bottom_right = ((col + 1) * 256, (row + 1) * 256)
    img_width = img_height = 256
    if bottom_right[0] > level_width:
        img_width = 256 - (bottom_right[0] - level_width)
    if bottom_right[1] > level_height:
        img_height = 256 - (bottom_right[1] - level_height)
    if img_width != 256 or img_height != 256:
        input_stream = StringIO(obj.read())
        im = Image.open(input_stream)
        new_image = im.crop((0, 0, img_width, img_height))
        result = StringIO()
        new_image.save(result, "PPM")
        content_length = result.tell()
        result.seek(0)
        content_type = "image/x-portable-anymap"
    else:
        result = obj
        content_type = obj.headers['Content-Type']
        content_length = obj.headers['Content-Length']

    time = datetime.utcnow() + timedelta(days=365)
    timestr = time.strftime('%a, %d %b %Y %H:%M:%S GMT')

    headers = [ # copy some headers for caching purposes
        ('Content-Length',		str(content_length)),
        ('Content-Type',		content_type),
        ('Last-Modified',		obj.headers['Last-Modified']),
        ('Expires',                     timestr),

        ('x-attr-gigapan_id',           str(id)),
        ('x-attr-gigapan_height',       str(info.get('height'))),
        ('x-attr-gigapan_width',        str(info.get('width'))),
        ('x-attr-gigapan_levels',       str(info.get('levels'))),
        ('x-attr-tile_level',           str(lvl)),
        ('x-attr-tile_col',             str(col)),
        ('x-attr-tile_row',             str(row)),
    ]
    start_response("200 OK", headers)
    return environ['wsgi.file_wrapper'](result, 65536)
