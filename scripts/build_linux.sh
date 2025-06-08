#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_linux Debug|RelWithDebInfo"
  exit 1
fi

cd ..
mkdir -p "build/linux/$1"
cd "build/linux/$1"
cmake -DCMAKE_BUILD_TYPE="$1" ../../..
make

cd ../../../dist/linux/"$1"
zip -r FireflySynth2-linux-vst3-"$1".zip FireflySynth2.vst3
zip -r FireflySynth2-linux-clap-"$1".zip FireflySynth2.clap
cd ../../../scripts