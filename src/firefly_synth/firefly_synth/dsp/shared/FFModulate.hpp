#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>

enum class FFModulationOpType { 
  Off,
  UPAdd, UPMul, UPStack, UPRemap,
  BPAdd, BPAdd2, BPMul, BPStack, BPRemap,
  PhaseWrap };

inline bool 
FFModulationOpTypeIsBipolar(FFModulationOpType opType)
{
  switch (opType)
  {
  case FFModulationOpType::UPAdd:
  case FFModulationOpType::UPMul:
  case FFModulationOpType::UPStack:
  case FFModulationOpType::UPRemap:
  case FFModulationOpType::PhaseWrap:
    return false;
  case FFModulationOpType::BPAdd:
  case FFModulationOpType::BPAdd2:
  case FFModulationOpType::BPMul:
  case FFModulationOpType::BPStack:
  case FFModulationOpType::BPRemap:
    return true;
  case FFModulationOpType::Off:
  default: FB_ASSERT(false); return false;
  }
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
FFModulateUPRemap(
  float source,
  float amount, float target)
{
  source = 1.0f - std::abs(FBToBipolar(source));
  return FFModulateUPMul(source, amount, target);
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
FFModulateBPAdd2(
  float source,
  float amount, float target)
{
  source = FBToBipolar(source) * 0.5f;
  return std::clamp(target + amount * source, 0.0f, 1.0f);
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
FFModulateBPRemap(
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
FFModulatePhaseWrap(
  float source,
  float amount, float target)
{
  return FBPhaseWrap(source + amount * target);
}

inline float
FFModulate(
  FFModulationOpType opType, float source, 
  float amount, float target)
{
  switch (opType)
  {
  case FFModulationOpType::UPAdd:
    return FFModulateUPAdd(source, amount, target);
  case FFModulationOpType::UPMul:
    return FFModulateUPMul(source, amount, target);
  case FFModulationOpType::UPStack:
    return FFModulateUPStack(source, amount, target);
  case FFModulationOpType::UPRemap:
    return FFModulateUPRemap(source, amount, target);
  case FFModulationOpType::BPAdd:
    return FFModulateBPAdd(source, amount, target);
  case FFModulationOpType::BPAdd2:
    return FFModulateBPAdd2(source, amount, target);
  case FFModulationOpType::BPMul:
    return FFModulateBPMul(source, amount, target);
  case FFModulationOpType::BPStack:
    return FFModulateBPStack(source, amount, target);
  case FFModulationOpType::BPRemap:
    return FFModulateBPRemap(source, amount, target);
  case FFModulationOpType::PhaseWrap: 
    return FFModulatePhaseWrap(source, amount, target);
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
FFModulateUPRemap(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  source = 1.0f - xsimd::abs(FBToBipolar(source));
  return FFModulateUPMul(source, amount, target);
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
FFModulateBPAdd2(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  source = FBToBipolar(source) * 0.5f;
  return xsimd::clip(target + amount * source, FBBatch<float>(0.0f), FBBatch<float>(1.0f));
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
FFModulateBPRemap(
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
FFModulatePhaseWrap(
  FBBatch<float> source,
  FBBatch<float> amount, FBBatch<float> target)
{
  return FBPhaseWrap(source + amount * target);
}

inline FBBatch<float>
FFModulate(
  FFModulationOpType opType, FBBatch<float> source, 
  FBBatch<float> amount, FBBatch<float> target)
{
  switch (opType)
  {
  case FFModulationOpType::UPAdd:
    return FFModulateUPAdd(source, amount, target);
  case FFModulationOpType::UPMul:
    return FFModulateUPMul(source, amount, target);
  case FFModulationOpType::UPStack:
    return FFModulateUPStack(source, amount, target);
  case FFModulationOpType::UPRemap:
    return FFModulateUPRemap(source, amount, target);
  case FFModulationOpType::BPAdd:
    return FFModulateBPAdd(source, amount, target);
  case FFModulationOpType::BPAdd2:
    return FFModulateBPAdd2(source, amount, target);
  case FFModulationOpType::BPMul:
    return FFModulateBPMul(source, amount, target);
  case FFModulationOpType::BPStack:
    return FFModulateBPStack(source, amount, target);
  case FFModulationOpType::BPRemap:
    return FFModulateBPRemap(source, amount, target);
  case FFModulationOpType::PhaseWrap:
    return FFModulatePhaseWrap(source, amount, target);
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