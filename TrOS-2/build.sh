#!/bin/bash

# Script for bulding and running the parts of the tros project
# Works on OSX, Linux and Windows (Mingw/git bash) using Docker to compile.
#
# REQUIREMENTS:
#   - Docker
#   - qemu-system-i386 needs to be present in ENV

# Builds the docker image used for compiling the souce code
function docker_build_image {
    exec "docker" "build" "-t" "tros" "."
}

function docker_cleanup {
    exec "docker" "rm" "$(docker ps -a -q)"
}

# Creates the bootable floppy disk image used by QEMU/Bosch
function image {
    exec "docker" "run" "tros" "image"
}

# Runs the docker container and builds the image
function build {
    exec "docker" "run" "--privileged" "-v" "/$PWD:/src" "tros"
    docker_cleanup
}

# Runs the docker container and rebuilds the image
function rebuild {
    exec "docker" "run" "tros" "rebuild"
    docker_cleanup
}

# Runs the floppy image
function run {
    exec "qemu-system-i386" "-fda" "build/tros.img" "-monitor" "stdio" "-m" "256"
    #-cpu 486 #-d cpu_reset
}

# Runs qemu in GDB remote debug mode
function debug {
    exec "qemu-system-i386" "-s" "-S" "-fda" "build/TrOS.img"
}

function select_func {
    echo $1
    build
}

if [ $# -eq 0 ]; then
    PS3="Choose an option: "
    select word in "dockerbuild" "image" "build" "rebuild" "run" "debug"
    do
        select_func $word
        break
    done
else
    select_func $1
fi

exit 0
