MXMLC=mxmlc
FDB=fdb

PROJECT=soma

DEBUG=-debug=true
MXMLC_FLAGS=${DEBUG} -default-size 400 400 -default-frame-rate 30 -compiler.source-path . ./p3d

all:	
	${MXMLC} ${MXMLC_FLAGS} -output ${PROJECT}.swf Main.as

debug:
	${FDB} ${PROJECT}.swf
