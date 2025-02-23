FASMARM=fasmarm
NDSTOOL=ndstool

ICON_FILE = img/icon.bmp
ROM_TITLE = "DS Prog"
OUT_FILE  = ds_prog.nds

arm_bins := nds7.bin nds9.bin

nds7.bin:
	$(FASMARM) nds7.asm
nds9.bin:
	$(FASMARM) nds9.asm

rom: $(arm_bins)
	$(NDSTOOL) \
		-7 nds7.bin \
		-9 nds9.bin \
		-b $(ICON_FILE) $(ROM_TITLE) \
		-c $(OUT_FILE)
	printf "Rom out file: %s\n" $(OUT_FILE)

clean:
	for file in $(arm_bins) $(OUT_FILE) ; do \
		rm -f "$$file" 2> /dev/null ; \
	done

