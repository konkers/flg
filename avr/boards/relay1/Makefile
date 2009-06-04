CROSS=avr
CROSS_CPU=atmega88

OBJS=main.o libavr-uart.o

TARGET=relay1.elf

all: ${TARGET}

${TARGET}: ${OBJS}
	${LINK}

include ${FLG_DIR}/rules.mk