CROSS=avr
CROSS_CPU=atmega8


OBJS=main.o ${LIBAVR_OBJS} ${LIBFLG_OBJS}

TARGETS=relay8-old.elf relay8-old.bin


all: ${TARGETS}

include ${FLG_DIR}/rules.mk

#must be after include for LIBAVR_OBJS and LIBFLG_OBJS to work
relay8-old.elf: ${OBJS}
	${LINK}

