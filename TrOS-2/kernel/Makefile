#Simple makefile for the X86 Stage 2 bootloader

BUILD = bin/
TARGET = kernel.bin

all: $(BUILD) $(TARGET)

$(TARGET):
	nasm -f bin kernel.asm -o $(BUILD)$(TARGET)

$(BUILD):
	mkdir $@

clean:
	rm -rf $(BUILD)
