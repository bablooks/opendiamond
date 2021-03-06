SUBDIRS = libfilter newscope tests

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = opendiamond.pc

nobase_noinst_PYTHON = \
	opendiamond/__init__.py \
	opendiamond/attributes.py \
	opendiamond/blobcache.py \
	opendiamond/bundle.py \
	opendiamond/config.py \
	opendiamond/hash.py \
	opendiamond/helpers.py \
	opendiamond/protocol.py \
	opendiamond/rpc.py \
	opendiamond/scope.py \
	opendiamond/xdr.py \
	opendiamond/blaster/__init__.py \
	opendiamond/blaster/__main__.py \
	opendiamond/blaster/cache.py \
	opendiamond/blaster/handlers.py \
	opendiamond/blaster/json.py \
	opendiamond/blaster/rpc.py \
	opendiamond/blaster/search.py \
	opendiamond/dataretriever/__init__.py \
	opendiamond/dataretriever/__main__.py \
	opendiamond/dataretriever/diamond_store.py \
	opendiamond/dataretriever/flickr_store.py \
	opendiamond/dataretriever/gigapan_store.py \
	opendiamond/dataretriever/mirage_store.py \
	opendiamond/dataretriever/proxy_store.py \
	opendiamond/dataretriever/pyramid.py \
	opendiamond/dataretriever/util.py \
	opendiamond/filter/__init__.py \
	opendiamond/filter/parameters.py \
	opendiamond/scopeserver/__init__.py \
	opendiamond/scopeserver/core/__init__.py \
	opendiamond/scopeserver/core/urls.py \
	opendiamond/scopeserver/core/views.py \
	opendiamond/scopeserver/flickr/__init__.py \
	opendiamond/scopeserver/flickr/forms.py \
	opendiamond/scopeserver/flickr/models.py \
	opendiamond/scopeserver/flickr/urls.py \
	opendiamond/scopeserver/flickr/views.py \
	opendiamond/scopeserver/gatekeeper/__init__.py \
	opendiamond/scopeserver/gatekeeper/admin.py \
	opendiamond/scopeserver/gatekeeper/forms.py \
	opendiamond/scopeserver/gatekeeper/models.py \
	opendiamond/scopeserver/gatekeeper/urls.py \
	opendiamond/scopeserver/gatekeeper/views.py \
	opendiamond/scopeserver/gigapan/__init__.py \
	opendiamond/scopeserver/gigapan/forms.py \
	opendiamond/scopeserver/gigapan/models.py \
	opendiamond/scopeserver/gigapan/urls.py \
	opendiamond/scopeserver/gigapan/views.py \
	opendiamond/scopeserver/mirage/__init__.py \
	opendiamond/scopeserver/mirage/forms.py \
	opendiamond/scopeserver/mirage/models.py \
	opendiamond/scopeserver/mirage/urls.py \
	opendiamond/scopeserver/mirage/views.py \
	opendiamond/server/__init__.py \
	opendiamond/server/__main__.py \
	opendiamond/server/child.py \
	opendiamond/server/filter.py \
	opendiamond/server/listen.py \
	opendiamond/server/object_.py \
	opendiamond/server/scopelist.py \
	opendiamond/server/search.py \
	opendiamond/server/sessionvars.py \
	opendiamond/server/statistics.py \
	opendiamond/server/resource.py \
	tests/test_cookies.py

nobase_dist_noinst_DATA = \
	opendiamond/bundle.xsd \
	opendiamond/bundle.xslt \
	opendiamond/hashmodule.c \
	opendiamond/blaster/static/json-blaster.js \
	opendiamond/blaster/static/json-blaster-autopause.js \
	opendiamond/blaster/static/sockjs.js \
	opendiamond/blaster/static/testui/jquery.js \
	opendiamond/blaster/static/testui/spinner.gif \
	opendiamond/blaster/static/testui/unknown.png \
	opendiamond/blaster/templates/testui/result.html \
	opendiamond/blaster/templates/testui/results.html \
	opendiamond/blaster/templates/testui/search.html \
	opendiamond/scopeserver/core/static/scopeserver/base.css \
	opendiamond/scopeserver/core/static/scopeserver/jquery.js \
	opendiamond/scopeserver/core/static/scopeserver/style.css \
	opendiamond/scopeserver/core/templates/registration/login.html \
	opendiamond/scopeserver/core/templates/scopeserver/base.html \
	opendiamond/scopeserver/core/templates/scopeserver/home.html \
	opendiamond/scopeserver/core/templates/scopeserver/simple_form.html \
	opendiamond/scopeserver/gatekeeper/templates/scopeserver/gatekeeper.html \
	opendiamond/scopeserver/gatekeeper/templates/scopeserver/manage.html \
	opendiamond/scopeserver/gigapan/templates/scopeserver/gigapan_browse.html \
	opendiamond/scopeserver/gigapan/templates/scopeserver/gigapan_search.html

dist_bin_SCRIPTS = \
	tools/cookiecutter \
	tools/diamond-bundle-predicate

dist_doc_DATA = doc/diamond-protocol.html
MAINTAINERCLEANFILES = doc/diamond-protocol.html

EXTRA_DIST = CREDITS LICENSE INSTALL README.txt \
	setup.py setup.cfg MANIFEST.in \
	requirements.txt \
	pylintrc tox.ini \
	doc/diamond-protocol.xml \
	tools/volcano

doc/diamond-protocol.html: doc/diamond-protocol.xml
	$(MKDIR_P) doc
	$(XML2RFC) --html -o $@ $^

all-local:
	(cd $(srcdir); $(PYTHON) setup.py build \
		--build-base $(shell readlink -f $(builddir))/build \
		--verbose)

check-local:
	@# don't worry when checks fail iff the source directory is readonly
	TOXENV=py27,lint $(TOX) || test ! -d .tox

install-exec-local:
	$(PYTHON) $(srcdir)/setup.py install \
		--prefix $(DESTDIR)$(prefix) \
		--single-version-externally-managed \
		--record $(DESTDIR)$(pyexecdir)/opendiamond/install_files.txt \
		--verbose

clean-local:
	rm -rf $(builddir)/build

uninstall-local:
	cat $(DESTDIR)$(pyexecdir)/opendiamond/install_files.txt | xargs rm -rf
	rm -rf $(DESTDIR)$(pyexecdir)/opendiamond
