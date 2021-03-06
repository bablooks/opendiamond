#!/usr/bin/python
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

import logging
import optparse
import sys

from paste import httpserver

import opendiamond
from opendiamond.config import DiamondConfig
from opendiamond.helpers import daemonize
from .util import DataRetriever

_server_version = "DataRetriever/" + opendiamond.__version__


def run():
    parser = optparse.OptionParser()
    parser.add_option("-f", "--config-file", dest="path")
    parser.add_option("-l", "--listen", dest="retriever_host",
                      help="Bind with the specified listen address")
    parser.add_option("-p", "--port", dest="retriever_port")
    parser.add_option("-d", "--daemonize", dest="daemonize",
                      action="store_true", default=False)
    (options, _) = parser.parse_args()

    # Load config
    kwargs = {}
    for opt in 'path', 'retriever_host', 'retriever_port':
        if getattr(options, opt) is not None:
            kwargs[opt] = getattr(options, opt)
    config = DiamondConfig(**kwargs)

    # Initialize logging
    logging.getLogger().addHandler(logging.StreamHandler())

    # Initialize app with configured store modules
    modules = {}
    for store in config.retriever_stores:
        modname = 'opendiamond.dataretriever.%s_store' % store
        __import__(modname, level=0)
        module = sys.modules[modname]
        if hasattr(module, 'init'):
            module.init(config)
        modules[module.BASEURL] = module.scope_app
    app = DataRetriever(modules)

    if options.daemonize:
        daemonize()

    print 'Enabled modules: ' + ', '.join(config.retriever_stores)
    httpserver.serve(app,
                     host=config.retriever_host,
                     port=config.retriever_port,
                     server_version=_server_version,
                     protocol_version='HTTP/1.1',
                     use_threadpool=True,
                     daemon_threads=True,
                     threadpool_workers=24)


if __name__ == '__main__':
    run()
