#Simple makefile for the X86 Stage 2 bootloader

BUILD = bin/
TARGET = krnldr.bin		#Have to remember, might be using FAT12 for a while

all: $(BUILD) $(TARGET)

$(TARGET):
	nasm -f bin kernload.asm -o $(BUILD)$(TARGET)

$(BUILD):
	mkdir $@

clean:
	rm -rf $(BUILD)
