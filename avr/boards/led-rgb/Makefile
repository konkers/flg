CROSS=avr
CROSS_CPU=atmega88

OBJS=main.o

TARGETS=led-rgb.elf led-rgb.bin

all: libflg.a libavr.a ${TARGETS}

include ${FLG_DIR}/rules.mk

# must be after include for LIBAVR_OBJS and LIBFLG_OBJS to work
led-rgb.elf: ${OBJS}
	${LINK}

