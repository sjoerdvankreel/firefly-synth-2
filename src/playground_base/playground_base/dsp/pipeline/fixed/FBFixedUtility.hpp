#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

inline int constexpr FBFixedFloatVectors = FBFixedBlockSamples / FBVectorFloatCount;
inline int constexpr FBFixedDoubleVectors = FBFixedBlockSamples / FBVectorDoubleCount;

struct alignas(sizeof(FBFloatVector)) FBFixedFloatArray 
{ std::array<float, FBFixedBlockSamples> data; };
struct alignas(sizeof(FBDoubleVector)) FBFixedDoubleArray 
{ std::array<double, FBFixedBlockSamples> data; };
struct alignas(sizeof(FBFloatVector)) FBFixedFloatAudioArray
{ std::array<FBFixedFloatArray, 2> data; };
struct alignas(sizeof(FBDoubleVector)) FBFixedDoubleAudioArray
{ std::array<FBFixedDoubleArray, 2> data; };