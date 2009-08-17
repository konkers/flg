OBJS=mongoose.o \
	colorserver.o \
	SimState.o \
	SimLink.o

TARGETS=brain

all: ${TARGETS}

brain: ${OBJS}
	${LINKXX}

brain: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

include ${FLG_DIR}/rules.mk
