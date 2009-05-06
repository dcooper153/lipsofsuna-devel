all:
	$(MAKE) -C libsquish all install

install:
	cp build/lib/libsquish.so ../build

clean:
	$(MAKE) -C libsquish clean
	rm -Rf build/lib/*
	rm -Rf build/include/*
