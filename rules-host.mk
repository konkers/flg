OPT = -g -O2
CFLAGS += ${OPT} -Wall -Werror -I${FLG_DIR}/lib/c/include -I/opt/local/include
CXXFLAGS += ${CFLAGS} -I${FLG_DIR}/lib/cxx/include -I${FLG_DIR}/lib/lua/src
LIBS += -lflg
LIBSXX += -lflgxx -llua ${LIBS} 
LDFLAGS += -L${FLG_DIR}/lib/c
LDXXFLAGS += ${LDFLAGS} -L${FLG_DIR}/lib/cxx -L${FLG_DIR}/lib/lua/src -L/opt/local/lib
RANLIB = ranlib

ifeq ("${SYSTEM}","Linux")
LIBS += -ldl -lpthread
endif
