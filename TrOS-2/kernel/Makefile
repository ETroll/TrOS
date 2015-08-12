# # # # #
# TrOS-2 Makefile
#
# 32 bit kernel
# # # # #

# COMPILATION
TOOLCHAIN = ../../Toolchains/gcc-i386-none-elf/bin/
GCC = $(TOOLCHAIN)i386-elf-gcc
COMPILER_ASM = nasm

# OUTPUT
DIR_BUILD = bin/
DIR_OBJECT = obj/
TARGET = kernel.elf
LIST = kernel.list
MAP = kernel.map


# CONFIGURATION
FLAGS = -Wall -std=c99 -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LINKER_FLAGS = -ffreestanding -nostdlib -Wl,-Map=$(DIR_OBJECT)$(MAP)
INCLUDES = ../include/
LINKER_MAP = kernel.ld
FOLDERS = . ds init hal irq sys mem drivers ../clib ../trell

# SOURCE FILES
SRC_GCC := $(foreach sdir, $(FOLDERS), $(wildcard $(sdir)/*.c))
SRC_AS := $(foreach sdir, $(FOLDERS), $(wildcard $(sdir)/*.asm))

# OBJECT FILES
OBJECTS_AS := $(patsubst %.asm, $(DIR_OBJECT)%.o, $(SRC_AS))
OBJECTS_GCC := $(patsubst %.c, $(DIR_OBJECT)%.o, $(SRC_GCC))
OBJECTS_ALL += $(OBJECTS_AS)
OBJECTS_ALL += $(OBJECTS_GCC)

all: $(LIST)

$(LIST) : $(TARGET)
	$(TOOLCHAIN)i386-elf-objdump -D $(DIR_BUILD)$(TARGET) > $(DIR_OBJECT)/$(LIST)

$(TARGET): $(DIR_BUILD) $(OBJECTS_AS) $(OBJECTS_GCC)
	$(GCC) $(LINKER_FLAGS) -T $(LINKER_MAP) $(addprefix $(DIR_OBJECT), $(notdir $(OBJECTS_ALL))) -o $(DIR_BUILD)$(TARGET)

$(OBJECTS_AS): $(DIR_OBJECT)
	$(COMPILER_ASM) -f elf $(patsubst $(DIR_OBJECT)%.o, %.asm, $@) -o $(patsubst $(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(OBJECTS_GCC): $(DIR_OBJECT)
	$(GCC) $(FLAGS) -I$(INCLUDES) -c $(patsubst $(DIR_OBJECT)%.o, %.c, $@) -o $(patsubst $(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(DIR_BUILD):
	mkdir $@

 $(DIR_OBJECT):
	mkdir $@

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_OBJECT)
