#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_mac Debug|Release"
  exit 1
fi

cd ..
mkdir -p "build/mac/$1"
cd "build/mac/$1"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_ARCH_TYPE=2 ../../..
make