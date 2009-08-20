SIM_OBJS=mongoose.o \
	FlameProtoThread.o \
	LedProtoThread.o \
	Soma.o \
	SimState.o \
	SimLink.o \
	sim.o

OBJS=${SIM_OBJS}

TARGETS=sim

all: ${TARGETS}

sim: ${SIM_OBJS}
	${LINKXX}

sim: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

include ${FLG_DIR}/rules.mk
