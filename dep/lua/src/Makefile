CFLAGS+=-g -Wall -O2 -fpic
TARGET=liblua5.1.so.0.0.0
OBJECTS=\
	lapi.o lcode.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o \
	lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o \
	lauxlib.o lbaselib.o ldblib.o liolib.o lmathlib.o loslib.o ltablib.o lstrlib.o \
	loadlib.o linit.o
all: $(TARGET)
clean:
	rm -f *.o liblua5.1.so*
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -shared -Wl,-soname -Wl,liblua5.1.so.0 $(OBJECTS) -o $@
	rm -f liblua5.1.so.0 && ln -s $@ liblua5.1.so.0
	rm -f liblua5.1.so && ln -s $@ liblua5.1.so
$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -DLUA_USE_LINUX -c -o $@ $<
