WAITER_OBJS=waiter.o
SENDER_OBJS=sender.o
OBJS=${WAITER_OBJS} ${SENDER_OBJS}

TARGETS=waiter sender

all: ${TARGETS}

waiter: ${WAITER_OBJS}
	${LINKXX}

sender: ${SENDER_OBJS}
	${LINKXX}

include ${FLG_DIR}/rules.mk
