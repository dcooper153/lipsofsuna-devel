
include config

SRC = alpha.cpp clusterfit.cpp colourblock.cpp colourfit.cpp colourset.cpp maths.cpp rangefit.cpp singlecolourfit.cpp squish.cpp
OBJ = $(SRC:%.cpp=%.o)
LIB = libsquish.so
CXXFLAGS+=-fPIC

all : $(LIB)

install : $(LIB)
	install squish.h $(INSTALL_DIR)/include 
	install $(LIB) $(INSTALL_DIR)/lib

uninstall:
	$(RM) $(INSTALL_DIR)/include/squish.h
	$(RM) $(INSTALL_DIR)/lib/$(LIB)

$(LIB) : $(OBJ)
	$(CXX) -shared $(CPPFLAGS) -I. $(CXXFLAGS) $(OBJ) -o $@

%.o : %.cpp
	$(CXX) $(CPPFLAGS) -I. $(CXXFLAGS) -o$@ -c $<

clean :
	$(RM) $(OBJ) $(LIB)



