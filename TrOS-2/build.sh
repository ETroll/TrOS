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
unamestr=`uname`

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
    local retval=$(echo $?)
    docker_cleanup
    if [ $retval -ne 0 ] ; then
       echo "make returned with exit code $retval, aborting!" >&2
       return $retval
    else
        echo " "
        echo "------ BUILD DONE ------"
        echo " "
        echo " "
        echo " "
    fi
}

# Runs the docker container and rebuilds the image
function rebuild {
    eval "docker run --privileged -v /$PWD:/src $IMAGENAME rebuild"
    docker_cleanup
}

# Runs the floppy image
function run {
    if [[ "$unamestr" == MINGW64_NT* ]]; then
        eval "qemu-system-i386 -fda build/tros.img -monitor stdio -serial file:CON -m 256 -d cpu_reset"
        qemu_cleanup
    else
        eval "qemu-system-i386 -fda build/tros.img -serial stdio -m 256"
    fi
}

# Runs qemu in GDB remote debug mode
function debug {
    if [[ "$unamestr" == MINGW64_NT* ]]; then
        eval "/c/Bochs-2.6.7/bochsdbg -q -f bochsrc.bxrc"
    else
        eval "qemu-system-i386 -s -S -fda build/tros.img"
    fi
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

        local _ret=$?
        if [ $_ret -ne 0 ] ; then
           echo "$arg returned with exit code $_ret, stopping action queue!"
           exit 1
        fi
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
