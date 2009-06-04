
ifeq ("${CROSS_CPU}","")
CROSS_CPU=atmega88
endif

CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size

OPT=-O2 -g

CFLAGS=-mmcu=${CROSS_CPU} ${OPT} -Wall -Werror \
	-I${FLG_DIR}/lib/c/include \
	-I${FLG_DIR}/avr/lib/include

LDFLAGS=-mmcu=${CROSS_CPU} 

LIBFLG_OBJS = libflg-proto.o libflg-crc8.o
LIBAVR_OBJS = libavr-uart.o

libavr-%.o: ${FLG_DIR}/avr/lib/%.c
	${CC} -c ${CFLAGS} -o $@ $^

libflg-%.o: ${FLG_DIR}/lib/c/%.c
	${CC} -c ${CFLAGS} -o $@ $^

%.bin: %.elf
	${OBJCOPY} -O binary $^ $@

%.lst: %.elf
	${OBJDUMP} --disassemble-all --source $^ > $@