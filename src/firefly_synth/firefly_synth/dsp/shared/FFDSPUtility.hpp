#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>

enum class FFModulationOpType { 
  Off, 
  Add, Mul, Stack, 
  BPAdd, BPMul, BPStack };

inline float
MultiplyClamp(
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

inline FBBatch<float>
FFApplyModulationStack(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return target + (1.0f - target) * source * amount;
}

inline FBBatch<float>
FFApplyModulationMul(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return (1.0f - amount) * target + amount * source * target;
}

inline FBBatch<float>
FFApplyModulationAdd(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return xsimd::clip(target + source * amount, FBBatch<float>(0.0f), FBBatch<float>(1.0f));
}

inline FBBatch<float>
FFApplyModulationBPMul(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto bpTarget = FBToBipolar(target);
  return FBToUnipolar((1.0f - amount) * bpTarget + amount * source * bpTarget);
}

inline FBBatch<float>
FFApplyModulationBPAdd(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto result = FBToBipolar(target) + FBToBipolar(source) * amount;
  return FBToUnipolar(xsimd::clip(result, FBBatch<float>(-1.0f), FBBatch<float>(1.0f)));
}

inline FBBatch<float>
FFApplyModulationBPStack(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  auto bpTarget = FBToBipolar(target);
  auto headroom = 1.0f - xsimd::abs(bpTarget);
  return FBToUnipolar(bpTarget + FBToBipolar(source) * amount * headroom);
}

inline FBBatch<float>
FFApplyModulation(
  FFModulationOpType opType, FBBatch<float> source, 
  FBBatch<float> amount, FBBatch<float> target)
{
  switch (opType)
  {
  case FFModulationOpType::Add: 
    return FFApplyModulationAdd(source, amount, target);
  case FFModulationOpType::Mul: 
    return FFApplyModulationMul(source, amount, target);
  case FFModulationOpType::Stack: 
    return FFApplyModulationStack(source, amount, target);
  case FFModulationOpType::BPAdd: 
    return FFApplyModulationBPAdd(source, amount, target);
  case FFModulationOpType::BPMul:
    return FFApplyModulationBPMul(source, amount, target);
  case FFModulationOpType::BPStack:
    return FFApplyModulationBPStack(source, amount, target);
  case FFModulationOpType::Off:
  default: 
    FB_ASSERT(false); return {};
  }
}