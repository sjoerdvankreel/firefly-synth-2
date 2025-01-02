#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <array>

struct alignas(sizeof(FBFloatVector)) FBFixedFloatArray 
{ std::array<float, FBFixedBlockSamples> data; };
struct alignas(sizeof(FBDoubleVector)) FBFixedDoubleArray 
{ std::array<double, FBFixedBlockSamples> data; };

typedef std::array<float, FBFixedBlockSamples> FBFixedFloatArray;
typedef std::array<double, FBFixedBlockSamples> FBFixedDoubleArray;
typedef std::array<FBFixedFloatArray, 2> FBFixedFloatAudioArray;
typedef std::array<FBFixedDoubleArray, 2> FBFixedDoubleAudioArray;