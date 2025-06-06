#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_linux Debug|Release"
  exit 1
fi

cd ..
mkdir -p "build/linux/$1"
cd "build/linux/$1"
cmake -DCMAKE_BUILD_TYPE="$1" ../../..
make
