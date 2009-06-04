DIRS = lib utils

all: dirs

.PHONY: avr
avr:
	@make FLG_DIR=${FLG_DIR} -C avr all || exit 1

clean-here:
	@make FLG_DIR=${FLG_DIR} -C avr clean || exit 1

include rules.mk