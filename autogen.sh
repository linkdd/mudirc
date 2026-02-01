#!/usr/bin/env bash

ROOTDIR=$(pwd)
BUILDDIR=$ROOTDIR/__build__
DISTDIR=$ROOTDIR/dist

CC=${CC:-clang}
MAKE=${MAKE:-make}

BUILD_TYPE="Debug"

while [ $# -gt 0 ]
do
  case "$1" in
    --release)
      BUILD_TYPE="Release"
      shift
      ;;
  esac
done

mkdir -p $BUILDDIR/${BUILD_TYPE}
cd $BUILDDIR/${BUILD_TYPE}

GENERATOR="Unix Makefiles"

cmake ../.. \
  -G "$GENERATOR" \
  -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
  -DCMAKE_MAKE_PROGRAM=$MAKE \
  -DCMAKE_C_COMPILER=$CC \
  -DCMAKE_INSTALL_PREFIX=$DISTDIR/${BUILD_TYPE}
