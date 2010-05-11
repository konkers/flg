SIM_OBJS=mongoose.o \
	Soma.o \
	SimState.o \
	SimLink.o \
	sim.o

BRAIN_OBJS=mongoose.o \
	Soma.o \
	brain.o

OBJS=${SIM_OBJS}

TARGETS=sim brain

all: ${TARGETS}

sim: ${SIM_OBJS}
	${LINKXX} -lxml2

brain: ${BRAIN_OBJS}
	${LINKXX} -lxml2 -lftdi

sim: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

brain: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

include ${FLG_DIR}/rules.mk
