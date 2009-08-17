OBJS=UdpLink.o TtyLink.o Proto.o Brain.o MapFileParser.o
TARGETS=libflgxx.a

all: ${TARGETS}

libflgxx.a: ${OBJS}
	${MKLIB}

include ${FLG_DIR}/rules.mk
