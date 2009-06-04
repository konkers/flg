WAITER_OBJS=waiter.o
SENDER_OBJS=sender.o
OBJS=${WAITER_OBJS} ${SENDER_OBJS}

TARGETS=waiter sender

all: ${TARGETS}

waiter: ${WAITER_OBJS}
	${LINKXX}

sender: ${SENDER_OBJS}
	${LINKXX}

waiter: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a
sender: ${FLG_DIR}/lib/cxx/libflgxx.a ${FLG_DIR}/lib/c/libflg.a


include ${FLG_DIR}/rules.mk
