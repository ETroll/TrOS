#!/bin/bash

# Script for bulding and running the parts of the tros project
# Works on OSX, Linux and Windows (Mingw/git bash) using Docker to compile.
#
# REQUIREMENTS:
#   - Docker
#   - qemu-system-i386 needs to be present in ENV
#   - bochs need to be present in ENV

# Builds the docker image used for compiling the souce code

IMAGENAME="tros"

function docker_build_image {
    eval "docker build -t $IMAGENAME ."
}

function docker_cleanup {
    #cleans up everything for now, not good.
    #docker rm $(docker ps -a -q)
    for arg in "$(docker ps -a -q)"; do
        eval "docker rm" $arg
    done

}

function qemu_cleanup {
    eval "rm trace-*"
}

# Creates the bootable floppy disk image used by QEMU/Bosch
function image {
    eval "docker run --privileged -v /$PWD:/src $IMAGENAME image"
    docker_cleanup
}

function tools {
    eval "docker run --privileged -v /$PWD:/src $IMAGENAME tools"
    docker_cleanup
}

# Runs the docker container and builds the image
function build {
    eval "docker run --privileged -v /$PWD:/src $IMAGENAME"
    docker_cleanup
}

# Runs the docker container and rebuilds the image
function rebuild {
    eval "docker run --privileged -v /$PWD:/src $IMAGENAME rebuild"
    docker_cleanup
}

# Runs the floppy image
function run {
    eval "qemu-system-i386 -fda build/tros.img -monitor stdio -m 256"
    #-cpu 486 #-d cpu_reset
    qemu_cleanup
}

# Runs qemu in GDB remote debug mode
function debug {
    #eval "qemu-system-i386 -s -S -fda build/tros.img"
    eval "/c/Bochs-2.6.7/bochsdbg -q -f bochsrc.bxrc"
    #qemu_cleanup
}

function select_func {
    for arg in "${@}"; do
        case $arg in
            dockerbuild)
                docker_build_image;;
            image)
                image;;
            tools)
                tools;;
            build)
                build;;
            rebuild)
                rebuild;;
            run)
                run;;
            debug)
                debug;;
            *)
                echo "Unknown parameter"
        esac
    done
    exit 1
}

if [ $# -eq 0 ]; then
    PS3="Choose an option: "
    select word in "dockerbuild" "image" "tools" "build" "rebuild" "run" "debug"
    do
        select_func $word
        break
    done
else
    select_func $@
fi

exit 0
