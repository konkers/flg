
ifeq ("${FLG_DIR}","")
FLG_DIR = ${shell pwd}
endif

OPT = -g -O2
CFLAGS += ${OPT} -Wall -Werror -I${FLG_DIR}/lib/c/include
CXXFLAGS += ${CFLAGS} -I${FLG_DIR}/lib/cxx/include
LIBS += -lflg
LIBSXX += -lflgxx ${LIBS} 
LDFLAGS += -L${FLG_DIR}/lib/c
LDXXFLAGS += ${LDFLAGS} -L${FLG_DIR}/lib/cxx

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
