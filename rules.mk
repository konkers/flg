
ifeq ("${FLG_DIR}","")
FLG_DIR = ${shell pwd}
endif

ifeq ("${CROSS}","avr")
include ${FLG_DIR}/rules-avr.mk
else
include ${FLG_DIR}/rules-host.mk
endif

LINK = ${CC} ${LDFLAGS} -o $@ $^ ${LIBS}
LINKXX = ${CXX} ${LDXXFLAGS} -o $@ $^ ${LIBSXX}

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
