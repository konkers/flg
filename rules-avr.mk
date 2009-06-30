
ifeq ("${CROSS_CPU}","")
CROSS_CPU=atmega88
endif

CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AR=avr-ar
RANLIB=avr-ranlib

OPT=-O2 -g

CFLAGS=-mmcu=${CROSS_CPU} ${OPT} -Wall -Werror \
	-I${FLG_DIR}/lib/c/include \
	-I${FLG_DIR}/avr/lib/include

LDFLAGS=-mmcu=${CROSS_CPU} -L.

LIBFLG_OBJS = libflg-proto.o libflg-crc8.o
LIBAVR_OBJS = libavr-uart.o libavr-config.o

LIBS=-lflg -lavr

libavr-%.o: ${FLG_DIR}/avr/lib/%.c
	@echo "  CC     " `basename $<`
	@${CC} -c ${CFLAGS} -o $@ $<

libflg-%.o: ${FLG_DIR}/lib/c/%.c
	@echo "  CC     " `basename $<`
	@${CC} -c ${CFLAGS} -o $@ $<

libflg.a: ${LIBFLG_OBJS}
	${MKLIB}

libavr.a: ${LIBAVR_OBJS}
	${MKLIB}

%.bin: %.elf
	@echo "  OBJCPY " $@; ${OBJCOPY} -O binary $^ $@

%.lst: %.elf
	${OBJDUMP} --disassemble-all --source $^ > $@

ifeq ("${CROSS_CPU}","atmega48")
AVRDUDE_CPU=m48
else ifeq ("${CROSS_CPU}","atmega88")
AVRDUDE_CPU=m88
else ifeq ("${CROSS_CPU}","atmega168")
AVRDUDE_CPU=m168
else ifeq ("${CROSS_CPU}","atmega8")
AVRDUDE_CPU=m8
FUSES=
endif

flash: flash-${basename ${firstword ${filter %.bin, ${TARGETS}}}}

flash-%: %.bin
	avrdude -p ${AVRDUDE_CPU} -c fg -e -U flash:w:$< ${FUSES}

clean-host:
	@rm -f ${LIBFLG_OBJS} ${LIBAVR_OBJS} libflg.a libavr.a

