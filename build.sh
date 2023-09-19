#!/usr/bin/env bash

ROOTDIR=$(pwd)
BUILD_CONFIGS="debug release"

for CONFIG in $BUILD_CONFIGS; do
    BUILDFOLDER=$ROOTDIR/build/$CONFIG
    DISTFOLDER=$ROOTDIR/dist/$CONFIG

    mkdir -p $BUILDFOLDER $DISTFOLDER

    cd $BUILDFOLDER

    cmake -DCMAKE_BUILD_TYPE=$CONFIG $ROOTDIR || exit 1

    make
    make install
done

