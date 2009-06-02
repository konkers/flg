
ifeq ("${FLG_DIR}","")
FLG_DIR = ${shell pwd}
endif

OPT = -g -O2
CFLAGS += ${OPT} -Wall -Werror -I${FLG_DIR}/lib/c/include
LIBS += -lflg
LDFLAGS += -L${FLG_DIR}

LINK = ${CC} ${LDFLAGS} -o $@
MKLIB=rm -f $@; ${AR} -cr $@ $^

.PHONY: dirs clean clean-here
dirs:
	@for dir in ${DIRS}; do \
		make FLG_DIR=${FLG_DIR} -C $$dir all || exit 1;\
	done

clean: clean-here
	rm -f ${OBJS} ${TARGETS}
	@for dir in ${DIRS}; do \
		make FLG_DIR=${FLG_DIR} -C $$dir clean || exit 1;\
	done
