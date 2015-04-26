#Simple makefile for the X86 Stage 1 bootloader

BUILD = bin/
TARGET = floppy.mbr

all: $(BUILD) $(TARGET)

$(TARGET):
	nasm -f bin floppy.asm -o $(BUILD)$(TARGET)

$(BUILD):
	mkdir $@

clean:
	rm -rf $(BUILD)
