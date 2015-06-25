# # # # #
# TrOS-2 Makefile
#
# 32 bit kernel
# # # # #

# COMPILATION
TOOLCHAIN = ~/Projects/Private/TrOS/Toolchains/gcc-i386-none-elf/bin/
GCC = $(TOOLCHAIN)i386-elf-gcc
COMPILER_ASM = nasm

# OUTPUT
DIR_BUILD = bin/
DIR_OBJECT = obj/
TARGET = kernel.elf

# CONFIGURATION
FLAGS = -Wall
LINKER_FLAGS = -ffreestanding -nostdlib
INCLUDES = ../include/
LINKER_MAP = kernel.ld
FOLDERS = .
SOURCE =

# SOURCE FILES
SRC_DIR := $(addprefix $(SOURCE), $(FOLDERS))
SRC_GCC := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.c))
SRC_AS := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.asm))

# OBJECT FILES
OBJECTS_AS := $(patsubst $(SOURCE)%.asm, $(DIR_OBJECT)%.o, $(SRC_AS))
OBJECTS_GCC := $(patsubst $(SOURCE)%.c, $(DIR_OBJECT)%.o, $(SRC_GCC))
OBJECTS_ALL += $(OBJECTS_AS)
OBJECTS_ALL += $(OBJECTS_GCC)

all: $(TARGET)

$(TARGET): $(DIR_BUILD) $(OBJECTS_AS) $(OBJECTS_GCC)
	$(GCC) $(LINKER_FLAGS) -T $(LINKER_MAP) $(addprefix $(DIR_OBJECT), $(notdir $(OBJECTS_ALL))) -o $(DIR_BUILD)$(TARGET)

$(OBJECTS_AS): $(DIR_OBJECT)
	$(COMPILER_ASM) -f elf $(patsubst $(DIR_OBJECT)%.o, $(SOURCE)%.asm, $@) -o $(patsubst $(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(OBJECTS_GCC): $(DIR_OBJECT)
	$(GCC) $(FLAGS) -I$(INCLUDES) -c $(patsubst $(DIR_OBJECT)%.o, $(SOURCE)%.c, $@) -o $(patsubst $(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(DIR_BUILD):
	mkdir $@

 $(DIR_OBJECT):
	mkdir $@

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_OBJECT)
