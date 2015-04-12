#Simple makefile for the X86 Stage 1 bootloader

BUILD = bin/
TARGET = stage1.bin

all: $(TARGET)

$(TARGET):
	nasm -f bin stage1.asm -o $(BUILD)$(TARGET)

$(BUILD):
	mkdir $@

clean:
	rm $(BUILD)$(TARGET)
