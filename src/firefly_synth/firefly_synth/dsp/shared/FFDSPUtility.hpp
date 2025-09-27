#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <cmath>

enum class FFModulationOpType { 
  Off, Remap,
  UPAdd, UPMul, UPStack, 
  BPAdd, BPMul, BPStack };

inline float
FFSoftClip10(float val)
{
  if (val > 1.0f || val < -1.0f)
    val = 10.0f * std::tanh(val * 0.1f);
  return val;
}

inline float
FFMultiplyClamp(
  float val, float mul, float min, float max)
{
  return std::clamp(val * mul, min, max);
}

inline float
FFGraphFilterFreqMultiplier(
  bool graph, float sampleRate, float maxFilterFreq)
{
  if (graph && sampleRate < (maxFilterFreq * 2.0f))
    return sampleRate / (maxFilterFreq * 2.0f);
  return 1.0f;
}

inline float 
KeyboardTrackingMultiplier(
  float key, float trackingKey, float trackingAmt)
{
  if (trackingAmt == 0.0f)
    return 1.0f;
  return std::pow(2.0f, (key - 60.0f + trackingKey) / 12.0f * trackingAmt);
}

inline float
FFModulateUPStack(
  float source,
  float amount, float target)
{
  return target + (1.0f - target) * source * amount;
}

inline float
FFModulateUPMul(
  float source,
  float amount, float target)
{
  return (1.0f - amount) * target + amount * source * target;
}

inline float
FFModulateUPAdd(
  float source,
  float amount, float target)
{
  return std::clamp(target + source * amount, 0.0f, 1.0f);
}

inline float
FFModulateBPMul(
  float source,
  float amount, float target)
{
  auto bpTarget = FBToBipolar(target);
  return FBToUnipolar((1.0f - amount) * bpTarget + amount * source * bpTarget);
}

inline float
FFModulateBPAdd(
  float source,
  float amount, float target)
{
  auto result = FBToBipolar(target) + FBToBipolar(source) * amount;
  return FBToUnipolar(std::clamp(result, -1.0f, 1.0f));
}

inline float
FFModulateBPStack(
  float source,
  float amount, float target)
{
  auto bpTarget = FBToBipolar(target);
  auto headroom = 1.0f - std::abs(bpTarget);
  return FBToUnipolar(bpTarget + FBToBipolar(source) * amount * headroom);
}

inline float
FFModulateRemap(
  float source,
  float amount, float target)
{
  bool sourceLtHalf = source < 0.5f;
  float lowMin = 0.0f;
  float lowMax = source * 2.0f;
  float highMin = -1.0f + 2.0f * source;
  float highMax = 1.0f;
  float min = sourceLtHalf ? lowMin : highMin;
  float max = sourceLtHalf ? lowMax : highMax;
  return ((1.0f - amount) * target) + (amount * (min + (max - min) * target));
}

inline float
FFModulate(
  FFModulationOpType opType, float source, 
  float amount, float target)
{
  switch (opType)
  {
  case FFModulationOpType::Remap:
    return FFModulateRemap(source, amount, target);
  case FFModulationOpType::UPAdd:
    return FFModulateUPAdd(source, amount, target);
  case FFModulationOpType::UPMul:
    return FFModulateUPMul(source, amount, target);
  case FFModulationOpType::UPStack:
    return FFModulateUPStack(source, amount, target);
  case FFModulationOpType::BPAdd: 
    return FFModulateBPAdd(source, amount, target);
  case FFModulationOpType::BPMul:
    return FFModulateBPMul(source, amount, target);
  case FFModulationOpType::BPStack:
    return FFModulateBPStack(source, amount, target);
  case FFModulationOpType::Off:
  default: 
    FB_ASSERT(false); return {};
  }
}

inline void
FFApplyModulation(
  FFModulationOpType opType, 
  float source,
  float amount,
  float& target)
{
  target = FFModulate(opType, source, amount, target);
}

inline FBBatch<float>
FFModulateUPStack(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return target + (1.0f - target) * source * amount;
}

inline FBBatch<float>
FFModulateUPMul(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return (1.0f - amount) * target + amount * source * target;
}

inline FBBatch<float>
FFModulateUPAdd(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return xsimd::clip(target + source * amount, FBBatch<float>(0.0f), FBBatch<float>(1.0f));
}

inline FBBatch<float>
FFModulateBPMul(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto bpTarget = FBToBipolar(target);
  return FBToUnipolar((1.0f - amount) * bpTarget + amount * source * bpTarget);
}

inline FBBatch<float>
FFModulateBPAdd(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto result = FBToBipolar(target) + FBToBipolar(source) * amount;
  return FBToUnipolar(xsimd::clip(result, FBBatch<float>(-1.0f), FBBatch<float>(1.0f)));
}

inline FBBatch<float>
FFModulateBPStack(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto bpTarget = FBToBipolar(target);
  auto headroom = 1.0f - xsimd::abs(bpTarget);
  return FBToUnipolar(bpTarget + FBToBipolar(source) * amount * headroom);
}

inline FBBatch<float>
FFModulateRemap(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto sourceLtHalf = xsimd::lt(source, FBBatch<float>(0.5f));
  auto lowMin = FBBatch<float>(0.0f);
  auto lowMax = source * FBBatch<float>(2.0f);
  auto highMin = -1.0f + 2.0f * source;
  auto highMax = FBBatch<float>(1.0f);
  auto min = xsimd::select(sourceLtHalf, lowMin, highMin);
  auto max = xsimd::select(sourceLtHalf, lowMax, highMax);
  return ((1.0f - amount) * target) + (amount * (min + (max - min) * target));
}

inline FBBatch<float>
FFModulate(
  FFModulationOpType opType, FBBatch<float> source, 
  FBBatch<float> amount, FBBatch<float> target)
{
  switch (opType)
  {
  case FFModulationOpType::Remap:
    return FFModulateRemap(source, amount, target);
  case FFModulationOpType::UPAdd:
    return FFModulateUPAdd(source, amount, target);
  case FFModulationOpType::UPMul:
    return FFModulateUPMul(source, amount, target);
  case FFModulationOpType::UPStack:
    return FFModulateUPStack(source, amount, target);
  case FFModulationOpType::BPAdd: 
    return FFModulateBPAdd(source, amount, target);
  case FFModulationOpType::BPMul:
    return FFModulateBPMul(source, amount, target);
  case FFModulationOpType::BPStack:
    return FFModulateBPStack(source, amount, target);
  case FFModulationOpType::Off:
  default: 
    FB_ASSERT(false); return {};
  }
}

inline void
FFApplyModulation(
  FFModulationOpType opType, 
  FBSArray<float, FBFixedBlockSamples> const& source,
  FBSArray<float, FBFixedBlockSamples> const& amount,
  FBSArray<float, FBFixedBlockSamples>& target)
{
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    target.Store(s, FFModulate(opType, source.Load(s), amount.Load(s), target.Load(s)));
}