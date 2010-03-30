SIM_OBJS=mongoose.o \
	Soma.o \
	SimState.o \
	SimLink.o \
	sim.o

OBJS=${SIM_OBJS}

TARGETS=sim

all: ${TARGETS}

sim: ${SIM_OBJS}
	${LINKXX} -lxml2

sim: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

include ${FLG_DIR}/rules.mk
