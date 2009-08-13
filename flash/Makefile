MXMLC=mxmlc
FDB=fdb

PROJECT=soma

#DEBUG=-debug=true
MXMLC_FLAGS=${DEBUG} -default-size 640 480 -default-frame-rate 30 \
	-default-background-color 0x000000 -compiler.source-path . ./p3d

TARGETS=${PROJECT}.swf

all:	
	${MXMLC} ${MXMLC_FLAGS} -output ${PROJECT}.swf Main.as

debug:
	${FDB} ${PROJECT}.swf

include ${FLG_DIR}/rules.mk