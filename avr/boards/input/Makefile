CROSS=avr
CROSS_CPU=atmega88

FLGMAIN=yes
FOSC=18432000UL

OBJS=main.o

TARGET=input
TARGETS=${TARGET}.elf ${TARGET}.bin

all: libflg.a libavr.a ${TARGETS}

include ${FLG_DIR}/rules.mk

# must be after include for LIBAVR_OBJS and LIBFLG_OBJS to work
${TARGET}.elf: ${OBJS} libflg.a libavr.a
	${LINK}

