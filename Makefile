DIRS = lib brain utils

all: dirs

.PHONY: avr
avr:
	@make FLG_DIR=${FLG_DIR} -C avr all || exit 1

clean-here:
	@make FLG_DIR=${FLG_DIR} -C avr clean || exit 1

sizes-here:
	@printf "%-15s %6s %6s\n" "target" "flash" "sram"
	@printf " ----------------------------\n" "target" "flash" "sram"
	@make FLG_DIR=${FLG_DIR} -C avr sizes || exit 1

flash-%: avr/boards/%
	@make FLG_DIR=${FLG_DIR} -C $^ flash || exit 1

include rules.mk
