WAITER_OBJS=waiter.o
SENDER_OBJS=sender.o
SENDER485_OBJS=sender485.o
BRAIN_OBJS=brain.o
LEDSEND_OBJS=ledsend.o
RATETEST_OBJS=test.o
OBJS=${WAITER_OBJS} ${SENDER_OBJS} ${SENDER485_OBJS} ${BRAIN_OBJS} ${LEDSEND_OBJS} ${RATETEST_OBJS}

TARGETS=waiter sender sender485 brain ledsend ratetest

all: ${TARGETS}

waiter: ${WAITER_OBJS}
	${LINKXX}

sender: ${SENDER_OBJS}
	${LINKXX}

sender485: ${SENDER485_OBJS}
	${LINKXX}

brain: ${BRAIN_OBJS}
	${LINKXX}

ledsend: ${LEDSEND_OBJS}
	${LINKXX}

ratetest: ${RATETEST_OBJS}
	${LINKXX} -lftdi



waiter: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
sender: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
sender485: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
brain: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
ledsend: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
ratetest: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

include ${FLG_DIR}/rules.mk
