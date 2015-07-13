#TrOS-2 Makefile

MODULES := bootloader/stage1 bootloader/stage2 kernel
FOLDERS := build build/tmp
SUBCLEAN = $(addsuffix .clean,$(MODULES))

.PHONY: $(MODULES)

all: $(FOLDERS) $(MODULES)

rebuild: clean all image

qemu: rebuild run

run:
	qemu-system-i386 -fda build/TrOS.img -monitor stdio #-cpu 486 #-d cpu_reset

debug:
	qemu-system-i386 -s -S -fda build/TrOS.img

$(MODULES):
	$(MAKE) -C $@
	cp -r $@/bin/*.* ./build/tmp

clean: $(SUBCLEAN)
	rm -rf build/

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

image:
	gcc buildtools/elfinfo.c -o build/elfinfo -std=c99
	rm build/tmp/*.mbr
	go run buildtools/diskimg.go -out=build/ -boot=bootloader/stage1/bin/floppy.mbr \
	-content=build/tmp/

$(FOLDERS):
	mkdir $@
