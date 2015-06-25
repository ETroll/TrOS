###
#
# Troll OS
#
# Recursive folder -> One target makefile
#
###

ARMGNU ?= arm-none-eabi
##ARCH = VPB
##ARCH = RPI
ARCH = RPI2

### Configuration
#COPS = -Wall -O2 -nostdlib -nostartfiles -march=armv6 -msoft-float -fPIC -mapcs-frame -marm -ffreestanding -D$(ARCH)
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -D$(ARCH)
OBJECT = Obj/
SOURCE = Src/
BUILD = Bin/
INCLUDES  = Src/Include/
LINKER = Linker/kernel-$(ARCH).ld
MODULES := . Kernel Kernel/* Kernel/Arch/* Kernel/Drivers/* Applications Applications/*


### Output:

TARGET = kernel.img
LIST = kernel.list
MAP = kernel.map
HEX = kernel.hex

### Build:

SRC_DIR := $(addprefix $(SOURCE), $(MODULES))

SRC_GCC := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.c))
SRC_AS := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.s))

OBJECTS_AS := $(patsubst $(SOURCE)%.s, $(OBJECT)%.o, $(SRC_AS))
OBJECTS_GCC := $(patsubst $(SOURCE)%.c, $(OBJECT)%.o, $(SRC_GCC))


OBJECTS_ALL += $(OBJECTS_AS)
OBJECTS_ALL += $(OBJECTS_GCC)

rebuild: clean all

serial: rebuild
	qemu-system-arm -M versatilepb -cpu arm1176 -nographic -kernel $(BUILD)$(TARGET)

graphic: rebuild
	qemu-system-arm -M versatilepb -cpu arm1176 -serial stdio -kernel $(BUILD)$(TARGET)

all: $(TARGET) $(LIST) $(HEX)

$(LIST) : $(OBJECT)output.elf
	$(ARMGNU)-objdump -D $(OBJECT)output.elf > $(BUILD)/$(LIST)

$(TARGET) : $(OBJECT)output.elf
	$(ARMGNU)-objcopy $(OBJECT)output.elf -O binary $(BUILD)/$(TARGET)

$(HEX) : $(OBJECT)output.elf
	$(ARMGNU)-objcopy $(OBJECT)output.elf -O ihex $(BUILD)/$(HEX)

$(OBJECT)output.elf : $(OBJECTS_AS) $(OBJECTS_GCC) $(BUILD)
	$(ARMGNU)-ld --no-undefined $(addprefix $(OBJECT), $(notdir $(OBJECTS_ALL))) -Map $(BUILD)/$(MAP) -o $(OBJECT)output.elf -T $(LINKER)

$(OBJECTS_AS): $(OBJECT)
	$(ARMGNU)-as $(patsubst $(OBJECT)%.o, $(SOURCE)%.s, $@) -o $(patsubst $(@D)%.o, $(OBJECT)%.o, $@)

$(OBJECTS_GCC): $(OBJECT)
	$(ARMGNU)-gcc $(COPS) -std=c99 -I$(INCLUDES) -c $(patsubst $(OBJECT)%.o, $(SOURCE)%.c, $@) -o $(patsubst $(@D)%.o, $(OBJECT)%.o, $@)

$(OBJECT):
	mkdir $@

$(BUILD):
	mkdir $@

# Rule to clean files.
clean :
	-rm -rf $(OBJECT)
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
