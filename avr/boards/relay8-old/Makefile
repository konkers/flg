CROSS=avr
CROSS_CPU=atmega8

FOSC=7372800UL
FLGMAIN=yes

OBJS=main.o

TARGETS=relay8-old.elf relay8-old.bin

all: ${TARGETS}

include ${FLG_DIR}/rules.mk

#must be after include for LIBAVR_OBJS and LIBFLG_OBJS to work
relay8-old.elf: ${OBJS} libflg.a libavr.a 
	${LINK}

