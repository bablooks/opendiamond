#!/usr/bin/env python
#
#  The OpenDiamond Platform for Interactive Search
#
#  Copyright (c) 2011 Carnegie Mellon University
#  All rights reserved.
#
#  This software is distributed under the terms of the Eclipse Public
#  License, Version 1.0 which can be found in the file named LICENSE.
#  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
#  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from optparse import OptionParser
import sys

from opendiamond.config import DiamondConfig
from opendiamond.server import DiamondServer

# Create option parser
# pylint: disable=invalid-name
parser = OptionParser()
attrs = set()


def add_option(*args, **kwargs):
    opt = parser.add_option(*args, **kwargs)
    attrs.add(opt.dest)


# Configure options
add_option('-d', dest='daemonize', action='store_false', default=True,
           help='do not run as a daemon')
add_option('-e', metavar='SPEC',
           dest='debug_filters', action='append', default=[],
           help='filter name/signature to run under debugger')
add_option('-E', metavar='COMMAND',
           dest='debug_command', action='store', default='valgrind',
           help='debug command to use with -e (default: valgrind)')
add_option('-f', dest='path',
           help='config file')
add_option('-n', dest='oneshot', action='store_true', default=False,
           help='do not fork for a new connection')


def run():
    opts, args = parser.parse_args()
    if args:
        parser.error('unrecognized command-line arguments')

    # Calculate DiamondConfig arguments
    kwargs = dict([(attr, getattr(opts, attr)) for attr in attrs])
    # If we are debugging, force single-threaded filter execution
    if kwargs['debug_filters']:
        kwargs['threads'] = 1

    # Create config object and server
    try:
        config = DiamondConfig(**kwargs)
        server = DiamondServer(config)
    except Exception, e:  # pylint: disable=broad-except
        print str(e)
        sys.exit(1)

    # Run the server
    server.run()


if __name__ == '__main__':
    run()
