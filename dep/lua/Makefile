PREFIX=../build
HEADERS=src/lua.h src/luaconf.h src/lualib.h src/lauxlib.h etc/lua.hpp
all:
	$(MAKE) -C src
clean:
	$(MAKE) -C src clean
install:
	mkdir -p $(PREFIX)/include/lua
	install $(HEADERS) $(PREFIX)/include/lua
	cp -a src/*.so* $(PREFIX)/lib
