#TrOS-2 Makefile

MODULES := bootloader/stage1 bootloader/stage2 kernel trell
FOLDERS := build build/tmp
SUBCLEAN = $(addsuffix .clean,$(MODULES))
IMAGE = build/tros.img
TMPMOUNT = /mnt/floppy
DISKTOOL = build/trfs

.PHONY: $(MODULES)

all: $(FOLDERS) $(MODULES) ramdisk image

rebuild: clean all image

$(MODULES):
	$(MAKE) -C $@
	cp -r ./$@/bin/* ./build/tmp

clean: $(SUBCLEAN)
	rm -rf build/

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

image:
	rm build/tmp/*.mbr
	rm $(IMAGE)
	mkdosfs -F 12 -n "TROS" -C $(IMAGE) 1440
	dd if=bootloader/stage1/bin/floppy.mbr of=$(IMAGE) bs=512 count=1 conv=notrunc
	losetup /dev/loop0 $(IMAGE)
	mkdir $(TMPMOUNT)
	mount /dev/loop0 $(TMPMOUNT) -t msdos -o "fat=12"
	cp -rfv build/tmp/* $(TMPMOUNT)
	umount $(TMPMOUNT)
	losetup -d /dev/loop0

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
