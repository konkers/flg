WAITER_OBJS=waiter.o
SENDER_OBJS=sender.o
BRAIN_OBJS=brain.o
OBJS=${WAITER_OBJS} ${SENDER_OBJS} ${BRAIN_OBJS}

TARGETS=waiter sender brain

all: ${TARGETS}

waiter: ${WAITER_OBJS}
	${LINKXX}

sender: ${SENDER_OBJS}
	${LINKXX}

brain: ${BRAIN_OBJS}
	${LINKXX}

waiter: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
sender: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a
brain: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a ${FLG_DIR}/lib/lua/src/liblua.a

include ${FLG_DIR}/rules.mk
