include objects.mk

OBJS=${LUA_CORE_OBJS} ${LUA_LIB_OBJS}
TARGETS=liblua.a

all: ${TARGETS}

liblua.a: ${OBJS}
	${MKLIB}

include ${FLG_DIR}/rules.mk