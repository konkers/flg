OPT = -g -O2
CFLAGS += ${OPT} -Wall -Werror -I${FLG_DIR}/lib/c/include
CXXFLAGS += ${CFLAGS} -I${FLG_DIR}/lib/cxx/include
LIBS += -lflg
LIBSXX += -lflgxx ${LIBS} 
LDFLAGS += -L${FLG_DIR}/lib/c
LDXXFLAGS += ${LDFLAGS} -L${FLG_DIR}/lib/cxx
