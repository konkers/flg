
ifeq ("${FLG_DIR}","")
FLG_DIR = ${shell pwd}
endif

ifeq ("${SUBDIR}","")
SUBDIR := "${shell basename $$PWD}"
endif

ifeq ("${CROSS}","avr")
include ${FLG_DIR}/rules-avr.mk
else
include ${FLG_DIR}/rules-host.mk
endif

LINK =   @echo "  LINK   " $@; ${CC} ${LDFLAGS} -o $@ $^ ${LIBS}
LINKXX = @echo "  LINK++ " $@; ${CXX} ${LDXXFLAGS} -o $@ $^ ${LIBSXX}

MKLIB=@echo "  MKLIB  " $@; rm -f $@; ${AR} -cr $@ $^; ${RANLIB} $@; echo

.PHONY: dirs clean clean-here clean-host dirname

dirname:
ifeq ("${DIRPRINTED}","")
	@echo 
	@echo "${SUBDIR}:"
	@DIRPRINTED=1
endif

%.o: %.c dirname
	@echo "  CC     " `basename $<`
	@${CC} -c ${CFLAGS} -o $@ $<

%.o: %.cc dirname
	@echo "  C++    " `basename $<`
	@${CXX} -c ${CXXFLAGS} -o $@ $<

dirs:
	@for dir in ${DIRS}; do \
		make FLG_DIR=${FLG_DIR} SUBDIR="${SUBDIR}/$$dir" -C $$dir all || exit 1;\
	done

clean: clean-here clean-host
	@echo "CLEAN  ${SUBDIR}"
	@rm -f ${OBJS} ${TARGETS}
	@for dir in ${DIRS}; do \
		make FLG_DIR=${FLG_DIR} SUBDIR="${SUBDIR}/$$dir" -C $$dir clean || exit 1;\
	done
