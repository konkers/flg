OPT = -g -O2
CFLAGS += ${OPT} -Wall -Werror -I${FLG_DIR}/lib/c/include
CXXFLAGS += ${CFLAGS} -I${FLG_DIR}/lib/cxx/include -I${FLG_DIR}/lib/lua/src
LIBS += -lflg
LIBSXX += -lflgxx -llua ${LIBS} 
LDFLAGS += -L${FLG_DIR}/lib/c
LDXXFLAGS += ${LDFLAGS} -L${FLG_DIR}/lib/cxx -L${FLG_DIR}/lib/lua/src
RANLIB = ranlib