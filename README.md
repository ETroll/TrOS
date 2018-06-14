# TrOS-2

Second iteration of my hobby operating system project. The first iteration was a "proof-of-concept" project using ARM (RaspberryPi). I learnt a whole lot and ended up wanting to learn more and try to dirty my hands with the x86 (i386) platform. So here I am. :)

## Bootloader

A custom 2 stage bootloader has been created. It finds kernel.elf and places it into memory and executes it. It sets up a simple pagedir where the first 1M is identity-mapped and then maps up memory for the kernel that are located at 0xC0000000. It then runs the kernel and passes along information regarding memory and stack for the kernel to use. So far it only supports ELF32 and FAT12. (I hope that I get to expand it for USB drives or CD-ROM or something soon.)

## Buildtools

The project have a dockerfile that creates a image with the compiler and binutils that are needed to compile it. Run the build shell script to build the project using the docker image.

## Kernel and userland

The kernel is a very simple and basic kernel for now. The following "features" are implemented
 * Multiboot support
 * Preemptive scheduling (Simple round-robin scheduling for now)
 * Memory management
 * VFS
 * Basic drivers
 * Userland / Ring 3
 * ELF32 Execution
 * UI (Trell)
 * Userland library
 * IPC (Using messagequeues)
 * Threading

Planned features:

 * Lua support in Trell



## Progress

Progress is slow. But here is the latest "screenshot" / progress-picture:

![trell-ui-components](https://user-images.githubusercontent.com/404305/28158605-82ef8e6c-67ba-11e7-886e-60d4f1adfdc4.gif)
