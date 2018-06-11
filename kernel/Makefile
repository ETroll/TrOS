# # # # #
# TrOS-2 Makefile
#
# 32 bit kernel
# # # # #

# COMPILATION
TOOLCHAIN = ../tools/gcc-i386-none-elf/bin/
GCC = $(TOOLCHAIN)i386-elf-gcc
COMPILER_ASM = nasm

# OUTPUT
DIR_BUILD = bin/
DIR_OBJECT = obj/
TARGET = kernel.elf
DEBUG = kernel_debug.elf
LIST = kernel.list
MAP = kernel.map


# CONFIGURATION
# -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer -gdwarf-2
#FLAGS = -g -Wall -std=c99 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-omit-frame-pointer #-fno-asynchronous-unwind-tables
FLAGS = -ggdb -Wall -std=c99 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -gdwarf-2 -fno-strict-aliasing
LINKER_FLAGS = -ffreestanding -nostdlib -Wl,-Map=$(DIR_OBJECT)$(MAP)
INCLUDES = ../include/
LINKER_MAP = kernel.ld
FOLDERS = . klib init hal irq sys mem minidrivers fs sched

# SOURCE FILES
SRC_GCC := $(foreach sdir, $(FOLDERS), $(wildcard $(sdir)/*.c))
SRC_AS := $(foreach sdir, $(FOLDERS), $(wildcard $(sdir)/*.asm))

# OBJECT FILES
OBJECTS_AS := $(patsubst %.asm, $(DIR_OBJECT)%.o, $(SRC_AS))
OBJECTS_GCC := $(patsubst %.c, $(DIR_OBJECT)%.o, $(SRC_GCC))
OBJECTS_ALL += $(OBJECTS_AS)
OBJECTS_ALL += $(OBJECTS_GCC)

all: clean $(LIST)

$(LIST) : $(TARGET)
	$(TOOLCHAIN)i386-elf-objdump -M intel -D $(DIR_BUILD)$(TARGET) > $(DIR_OBJECT)/$(LIST)

$(TARGET) : $(DIR_BUILD) $(DEBUG)
	$(TOOLCHAIN)i386-elf-strip --strip-unneeded $(DIR_OBJECT)$(DEBUG) -o $(DIR_BUILD)$(TARGET)

$(DEBUG): $(OBJECTS_AS) $(OBJECTS_GCC)
	$(GCC) $(LINKER_FLAGS) -T $(LINKER_MAP) $(OBJECTS_ALL) -o $(DIR_OBJECT)$(DEBUG)

$(OBJECTS_AS): $(DIR_OBJECT)
	@mkdir -p $(@D)
	$(COMPILER_ASM) -f elf $(patsubst $(DIR_OBJECT)%.o, %.asm, $@) -o $(patsubst .$(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(OBJECTS_GCC): $(DIR_OBJECT)
	@mkdir -p $(@D)
	$(GCC) $(FLAGS) -I$(INCLUDES) -c $(patsubst $(DIR_OBJECT)%.o, %.c, $@) -o $(patsubst .$(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(DIR_BUILD):
	mkdir $@

 $(DIR_OBJECT):
	mkdir $@

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_OBJECT)
