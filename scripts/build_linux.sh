#!/bin/bash
set -e

if [ "$#" -ne 3 ]; then
  echo "Usage: build_linux Debug|RelWithDebInfo|Release 0|1 (warn as error) 0|1 (enable asan)"
  exit 1
fi

cd ..
mkdir -p "build/linux/$1"
cd "build/linux/$1"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_WARN_AS_ERROR="$2" -DFB_ENABLE_ASAN="$3" ../../..
make

cd ../../../dist/linux/"$1"
zip -r FireflySynth2-linux-vst3-"$1".zip FireflySynth2.vst3
zip -r FireflySynth2-linux-clap-"$1".zip FireflySynth2.clap
cd ../../../scripts