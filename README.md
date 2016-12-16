## TrOS-2

Second iteration of my hobby operating system project. In this iteration I have used what I learnt in the first iteration and tried to port it to the X86 (i386) platform.

### Bootloader

This is a custom 2 stage bootloader for FAT12 floppy disks. It sets up a simple GDT and switches to 32 bit mode. It supports kernels compiled to ELF32 format.

### Buildtools

Some build tools have been created using go-lang. These tools create a floppy disk image and copy the binaries on to it. There is also a script that can be used to install a cross-compile toolchain on OSX 10.10. The build tools currently only work with OSX 10.9 and above.

### Kernel

The TrOS-2 kernel is a hybrid 32bit kernel. It has the following features atm:

 - Multiboot support
 - :: TODO! Moar features!

Planned features:

 - Preemptive scheduling (simple scheuler)
 - Memory management and protection
 - Userland
 - Integrated Lua support (Integrated in to shell)

### Userland

There is a planned minimal libc planned, and this is the location for the applications residing in the userland would be. If I had any.. :P
