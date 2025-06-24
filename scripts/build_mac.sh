#!/bin/bash
set -e

if [ "$1" == "" ]; then
  echo "Usage: build_mac Debug|RelWithDebInfo|Release 0|1 (warn as error)"
  exit 1
fi
if [ "$2" == "" ]; then
  echo "Usage: build_mac Debug|RelWithDebInfo|Release 0|1 (warn as error)"
  exit 1
fi

cd ..
mkdir -p "build/mac/$1"
cd "build/mac/$1"
cmake -DCMAKE_BUILD_TYPE="$1" -DFB_WARN_AS_ERROR="$2" ../../..
make

cd ../../../dist/Darwin/"$1"
zip -r FireflySynth2-mac-vst3-"$1".zip FireflySynth2.vst3
zip -r FireflySynth2-mac-clap-"$1".zip FireflySynth2.clap
cd ../../../scripts