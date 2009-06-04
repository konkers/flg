CROSS=avr
CROSS_CPU=atmega8

OBJS=main.o libavr-uart.o

TARGET=relay8-old.elf

all: ${TARGET}

${TARGET}: ${OBJS}
	${LINK}

include ${FLG_DIR}/rules.mk