OPT = -g -O2
CFLAGS += ${OPT} -Wall -Werror -I${FLG_DIR}/lib/c/include
CXXFLAGS += ${CFLAGS} -I${FLG_DIR}/lib/cxx/include -I${FLG_DIR}/lib/lua/src
LIBS += -lflg
LIBSXX += -lflgxx -llua ${LIBS} 
LDFLAGS += -L${FLG_DIR}/lib/c
LDXXFLAGS += ${LDFLAGS} -L${FLG_DIR}/lib/cxx -L${FLG_DIR}/lib/lua/src 
RANLIB = ranlib

ifeq ("${SYSTEM}","Linux")
CFLAGS += -I/usr/include/libxml2
LIBS += -ldl -lpthread
endif

ifeq ("${SYSTEM}","Darwin")
CFLAGS += -I/opt/local/include -I/opt/local/include/libxml2
LDFLAGS += -L/opt/local/lib
endif
