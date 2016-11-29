#TrOS-2 Makefile

MODULES := bootloader/stage1 bootloader/stage2 kernel
USERLAND := trell
FOLDERS := build build/tmp build/tmp/bin
SUBCLEAN = $(addsuffix .clean,$(MODULES))
IMAGE = build/tros.img
DISKTOOL = build/trfs


.PHONY: $(MODULES) $(USERLAND) $(IMAGE)

all: $(FOLDERS) $(MODULES) $(USERLAND) ramdisk $(IMAGE)

rebuild: clean all

$(MODULES):
	$(MAKE) -C $@
	cp -r ./$@/bin/* ./build/tmp

$(USERLAND):
	$(MAKE) -C $@
	cp -r ./$@/bin/* ./build/tmp/bin

clean: $(SUBCLEAN)
	rm -rf build/

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

$(IMAGE):
	rm build/tmp/*.mbr
	rm -f $@
	mkdosfs -n "TROS" -C $@ 1440
	dd if=bootloader/stage1/bin/floppy.mbr of=$@ bs=512 count=1 conv=notrunc
	mcopy -i $@ ./build/tmp/* ::

ramdisk: tools
	$(DISKTOOL) build/tmp/initrd -c
	#echo "ABCD1234" > build/tmp/test
	# $(DISKTOOL) build/tmp/initrd -a build/elfinfo
	# $(DISKTOOL) build/tmp/initrd -l

tools: $(FOLDERS)
	gcc buildtools/elfinfo.c -o build/elfinfo -std=c99
	gcc buildtools/initrd.c -o $(DISKTOOL) -std=c99

$(FOLDERS):
	mkdir $@
