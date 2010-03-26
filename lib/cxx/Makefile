OBJS=UdpLink.o TtyLink.o FtdiLink.o Proto.o Brain.o \
	MapFileParser.o RingBuff.o Thread.o
TARGETS=libflgxx.a

all: ${TARGETS}

libflgxx.a: ${OBJS}
	${MKLIB}

include ${FLG_DIR}/rules.mk
