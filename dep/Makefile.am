INSTALLDIR=$(PWD)/build

all-local: build/lib/libsquish.so build/lib/libgrapple-0.9.so.2 build/lib/liblua5.1.so.0

clean-local:
	rm -Rf build/include/*
	rm -Rf build/lib/*
	$(MAKE) -C libsquish clean
	$(MAKE) -C lua clean
	if test -f libgrapple/Makefile; then $(MAKE) -C libgrapple distclean; fi

dist-hook:
	cp -R $(srcdir)/libgrapple $(distdir)
	cp -R $(srcdir)/libsquish $(distdir)
	cp -R $(srcdir)/lua $(distdir)
	mkdir -p $(distdir)/build/include
	mkdir -p $(distdir)/build/lib

install-exec-local:
	cp build/lib/libgrapple-0.9.so.2 $(DESTDIR)$(libdir)
	cp build/lib/libsquish.so $(DESTDIR)$(libdir)
	cp build/lib/liblua5.1.so.0 $(DESTDIR)$(libdir)

uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libgrapple-0.9.so.2
	rm -f $(DESTDIR)$(libdir)/libsquish.so
	rm -f $(DESTDIR)$(libdir)/liblua5.1.so.0

build/lib/libsquish.so:
	$(MAKE) -C libsquish all install
	cp build/lib/libsquish.so ../build/lib

build/lib/libgrapple-0.9.so.2:
	(cd libgrapple && ./configure --prefix=$(INSTALLDIR) && make && make install)

build/lib/liblua5.1.so.0:
	(cd lua && make && make install)	
