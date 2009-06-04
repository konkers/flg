CROSS=avr
CROSS_CPU=atmega88

OBJS=main.o

TARGETS=relay1.elf relay1.bin

all: libflg.a libavr.a ${TARGETS}

include ${FLG_DIR}/rules.mk

#must be after include for LIBAVR_OBJS and LIBFLG_OBJS to work
relay1.elf: ${OBJS}
	${LINK}
