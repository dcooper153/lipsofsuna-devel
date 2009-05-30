INSTALLDIR=$(PWD)/build

all-local: build/lib/libsquish.so build/lib/libgrapple-0.9.so.2 build/lib/liblua5.1.so.0 build/lib/libbulletdynamics.so.0

clean-local:
	rm -Rf build/include/*
	rm -Rf build/lib/*
	$(MAKE) -C libsquish clean
	$(MAKE) -C lua clean
	if test -f libgrapple/Makefile; then $(MAKE) -C libgrapple distclean; fi
	if test -f bullet/Makefile; then $(MAKE) -C bullet distclean; fi

dist-hook: clean-local
	cp -R $(srcdir)/libgrapple $(distdir)
	cp -R $(srcdir)/libsquish $(distdir)
	cp -R $(srcdir)/lua $(distdir)
	cp -R $(srcdir)/bullet $(distdir)
	mkdir -p $(distdir)/build/include
	mkdir -p $(distdir)/build/lib

install-exec-local:
	mkdir -p $(DESTDIR)$(libdir)/
	cp build/lib/libgrapple-0.9.so.2 $(DESTDIR)$(libdir)/
	cp build/lib/libsquish.so $(DESTDIR)$(libdir)/
	cp build/lib/liblua5.1.so.0 $(DESTDIR)$(libdir)/
	cp build/lib/libbulletcollision.so.0 $(DESTDIR)$(libdir)/
	cp build/lib/libbulletdynamics.so.0 $(DESTDIR)$(libdir)/
	cp build/lib/libbulletmath.so.0 $(DESTDIR)$(libdir)/

uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libgrapple-0.9.so.2
	rm -f $(DESTDIR)$(libdir)/libsquish.so
	rm -f $(DESTDIR)$(libdir)/liblua5.1.so.0
	rm -f $(DESTDIR)$(libdir)/libbulletcollision.so.0
	rm -f $(DESTDIR)$(libdir)/libbulletdynamics.so.0
	rm -f $(DESTDIR)$(libdir)/libbulletmath.so.0

build/lib/libbulletdynamics.so.0:
	(cd bullet && ./configure --prefix=$(INSTALLDIR) --disable-demos && \
		$(MAKE) $(AM_MAKEFLAGS) && \
		$(MAKE) $(AM_MAKEFLAGS) install)

build/lib/libgrapple-0.9.so.2:
	(cd libgrapple && ./configure --prefix=$(INSTALLDIR) && \
		$(MAKE) $(AM_MAKEFLAGS) && \
		$(MAKE) $(AM_MAKEFLAGS) install)

build/lib/liblua5.1.so.0:
	$(MAKE) $(AM_MAKEFLAGS) -C lua
	$(MAKE) $(AM_MAKEFLAGS) -C lua install

build/lib/libsquish.so:
	$(MAKE) $(AM_MAKEFLAGS) -C libsquish all install
