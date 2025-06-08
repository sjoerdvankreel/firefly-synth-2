#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_mac Debug|RelWithDebInfo"
  exit 1
fi

cd ..
mkdir -p "build/mac/$1"
cd "build/mac/$1"
cmake -DCMAKE_BUILD_TYPE="$1" ../../..
make

cd ../../../dist/Darwin/"$1"
zip -r FireflySynth2-mac-vst3-"$1".zip FireflySynth2.vst3
zip -r FireflySynth2-mac-clap-"$1".zip FireflySynth2.clap
cd ../../../scripts