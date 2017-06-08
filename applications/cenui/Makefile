
# COMPILATION
TOOLCHAIN = ../../tools/gcc-i386-none-elf/bin/
GCC = $(TOOLCHAIN)i386-elf-gcc
LD = $(TOOLCHAIN)i386-elf-ld

# OUTPUT
DIR_BUILD = bin/
DIR_OBJECT = obj/
TARGET = cenui


# CONFIGURATION
FLAGS = -Wall -std=c99 -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LINKER_FLAGS = -ffreestanding -nostdlib
INCLUDES = ../../include/
LINKER_MAP = flatbinary.ld
FOLDERS = ../../libraries/libc ../../libraries/trlib .

# SOURCE FILES
SRC_GCC := $(foreach sdir, $(FOLDERS), $(wildcard $(sdir)/*.c))

# OBJECT FILES
OBJECTS_GCC := $(patsubst %.c, $(DIR_OBJECT)%.o, $(SRC_GCC))
OBJECTS_ALL += $(OBJECTS_GCC)

all: $(TARGET)
	$(TOOLCHAIN)i386-elf-objdump -M intel -D $(DIR_BUILD)$(TARGET) > $(DIR_OBJECT)/$(TARGET).list
	# $(TOOLCHAIN)i386-elf-strip --strip-unneeded $(DIR_BUILD)$(TARGET)

$(TARGET): $(DIR_BUILD) $(OBJECTS_GCC)
	$(GCC) $(LINKER_FLAGS) -T $(LINKER_MAP) $(addprefix $(DIR_OBJECT), $(notdir $(OBJECTS_ALL))) -o $(DIR_BUILD)/$(TARGET)

$(OBJECTS_GCC): $(DIR_OBJECT)
	$(GCC) $(FLAGS) -I$(INCLUDES) -c $(patsubst $(DIR_OBJECT)%.o, %.c, $@) -o $(patsubst $(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(DIR_BUILD):
	mkdir $@

 $(DIR_OBJECT):
	mkdir $@

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_OBJECT)
