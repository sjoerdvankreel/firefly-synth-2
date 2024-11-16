#pragma once
#include <playground_plug/base/dsp/FBBuffers.hpp>
#include <playground_plug/plug/shared/FFConfiguration.hpp>

typedef FBMonoBuffer<FF_BUFFER_SIZE> FFFixedMonoBuffer;
typedef FBStereoBuffer<FF_BUFFER_SIZE> FFFixedStereoBuffer;