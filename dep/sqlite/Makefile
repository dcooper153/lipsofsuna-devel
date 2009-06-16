INSTALL_DIR=../build
TARGET=libsqlite3.so.0

all: $(TARGET)

clean:
	$(RM) $(TARGET)

install: all
	mkdir -p $(INSTALL_DIR)/include/sqlite
	mkdir -p $(INSTALL_DIR)/lib
	install sqlite3.h $(INSTALL_DIR)/include/sqlite
	install sqlite3ext.h $(INSTALL_DIR)/include/sqlite
	install $(TARGET) $(INSTALL_DIR)/lib
	install $(TARGET) $(INSTALL_DIR)/lib/libsqlite3.so

uninstall:
	$(RM) $(INSTALL_DIR)/include/sqlite/sqlite3.h
	$(RM) $(INSTALL_DIR)/include/sqlite/sqlite3ext.h
	$(RM) $(INSTALL_DIR)/lib/$(TARGET)

$(TARGET): sqlite3.c sqlite3.h sqlite3ext.h
	$(CC) $(CFLAGS) -shared -fPIC sqlite3.c -o $@
