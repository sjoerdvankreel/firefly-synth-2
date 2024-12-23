#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_linux Debug|Release"
  exit 1
fi

cd ..
mkdir -p "build/linux/$1/SSE"
cd "build/linux/$1/SSE"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_USE_SSE=1 -DFB_USE_AVX=0 ../../../..
make