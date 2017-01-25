#TrOS-2 Makefile

KERNEL := bootloader/bootsector bootloader/loader kernel
USERLAND := trell
FOLDERS := build build/tmp build/tmp/bin tools
SUBCLEAN = $(addsuffix .clean,$(KERNEL))
IMAGE = build/tros.img
DISKTOOL = build/trfs
TOOLCHAINDEST = $(shell pwd)/tools/gcc-i386-none-elf


.PHONY: $(KERNEL) $(USERLAND) $(IMAGE)

all: $(FOLDERS) $(KERNEL) $(USERLAND)

rebuild: clean all

user: $(USERLAND)

kernel: $(KERNEL)

$(KERNEL):
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
	rm -f build/tmp/*.mbr
	rm -f $@
	/sbin/mkdosfs -n "TROS" -C $@ 1440
	dd if=bootloader/bootsector/bin/floppy.mbr of=$@ bs=512 count=1 conv=notrunc
	mcopy -i $@ ./build/tmp/* ::
	@echo "\n\n\n------------- Created Image -------------\n\n\n"

run: all qemu
qemu: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE) -serial stdio -m 256 -d cpu_reset

debug: all bochs
bochs: $(IMAGE)
	tools/bochs/bochs -q -f bochsrc.bxrc

toolchain: $(FOLDERS)
	sudo apt-get install gcc g++ nasm wget dosfstools mtools libmpc-dev
	# cd tools && \
	# 	wget http://vps1.trollcode.no/i386-elf-gcc-WSL-bin.tar.gz && \
	# 	tar xvzf i386-elf-gcc-WSL-bin.tar.gz && \
	# 	mv i386-gcc-WSL i386-elf-gcc && \
	# 	rm i386-elf-gcc-WSL-bin.tar.gz
	cd tools && \
		wget http://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz \
		&& tar xvzf binutils-2.24.tar.gz \
		&& cd binutils-2.24 \
		&& ./configure --prefix=$(TOOLCHAINDEST) --target=i386-elf --disable-nls --disable-werror\
		&& make \
		&& make install \
		&& cd .. \
		&& rm -rf binutils-2.24 \
		&& rm binutils-2.24.tar.gz
	cd tools && \
		wget http://ftp.gnu.org/gnu/gcc/gcc-4.9.0/gcc-4.9.0.tar.gz \
	    && tar xvzf gcc-4.9.0.tar.gz \
	    && cd gcc-4.9.0 \
	    && ./configure --prefix=$(TOOLCHAINDEST) --target=i386-elf --disable-nls --enable-languages=c --without-headers --disable-werror\
	    && make all-gcc\
	    && make install-gcc\
	    && cd .. \
	    && rm -rf gcc-4.9.0 \
	    && rm gcc-4.9.0.tar.gz

	# cd tools && \
	# 	wget http://vps1.trollcode.no/bochs-2.6.8-WSL-bin.tar.gz && \
	# 	tar xvzf bochs-2.6.8-WSL-bin.tar.gz && \
	# 	rm bochs-2.6.8-WSL-bin.tar.gz
	sudo apt-get autoremove

$(FOLDERS):
	mkdir $@
