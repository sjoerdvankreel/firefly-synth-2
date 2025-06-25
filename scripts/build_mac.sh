#!/bin/bash
set -e

if [ "$#" -ne 3 ]; then
  echo "Usage: build_mac Debug|RelWithDebInfo|Release 0|1 (warn as error) 0|1 (enable asan)"
  exit 1
fi

cd ..
mkdir -p "build/mac/$1"
cd "build/mac/$1"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_WARN_AS_ERROR="$2" -DFB_ENABLE_ASAN="$3" ../../..
make

cd ../../../dist/Darwin/"$1"
zip -r FireflySynth2-mac-vst3-"$1".zip FireflySynth2.vst3
zip -r FireflySynth2-mac-clap-"$1".zip FireflySynth2.clap
cd ../../../scripts