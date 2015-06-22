#TrOS-2 Makefile

MODULES := bootloader/stage1 bootloader/stage2
FOLDERS := build build/tmp
SUBCLEAN = $(addsuffix .clean,$(MODULES))

.PHONY: $(MODULES)

rebuild: clean all

all: image

qemu: rebuild run

run:
	qemu-system-i386 -fda build/TrOS.img

$(MODULES):
	$(MAKE) -C $@
	cp -r $@/bin/*.* ./build/tmp

clean: $(SUBCLEAN)
	rm -rf build/

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

image: $(FOLDERS) $(MODULES)
	rm build/tmp/*.mbr
	go run buildtools/diskimg.go -out=build/ -boot=bootloader/stage1/bin/floppy.mbr \
	-content=build/tmp/

$(FOLDERS):
	mkdir $@
