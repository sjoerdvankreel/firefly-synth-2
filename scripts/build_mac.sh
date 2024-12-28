#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_mac Debug|Release"
  exit 1
fi

cd ..
mkdir -p "build/mac/$1/universal"
cd "build/mac/$1/universal"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_ARCH_TYPE=3 ../../../..
make