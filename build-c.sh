#!/usr/bin/env bash
usage="Usage: $0 [g] [b [target]]  \n g cmake gen \n b cmake build\n"

function buildAll() {
    cd cmake-tmp
    cmake   --build .
}

function buildTarget() {
    cd cmake-tmp
    cmake --build . --target $1
    cd ..
}

function cmakeGen() {

    rm -rf cmake-tmp 
    mkdir cmake-tmp
    cd cmake-tmp
    cmake ..
    cd ..
}


gen="0"
build="0"
target=""
cmd=""
while (( "$#" )); do
    if [  "x$1"  ==  "xg" ]; then    
        gen="1"
        cmd="1"
    else
        if [  "x$1"  ==  "xb" ]; then    
            build="1"
            cmd="1"
        else
            target=$1 
        fi
    fi
    shift
done


if [ $gen == "1" ]; then
    cmakeGen
fi

if [ $build == "1" ]; then
    if [ "x$target" == "x" ]; then 
        buildAll
    else
        buildTarget $target
    fi
fi




