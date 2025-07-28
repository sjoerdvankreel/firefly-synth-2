#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>
#include <cstdint>

struct FBStaticModule; 
template <bool Global>
struct LFOGraphRenderData;

struct FFLFOExchangeState final:
public FBModuleProcMultiExchangeState<FFLFOBlockCount + 1>
{
  std::array<float, FFLFOBlockCount> phases = {};
  std::array<std::uint32_t, FFLFOBlockCount> noiseState = {};
  std::array<std::uint32_t, FFLFOBlockCount> smoothNoiseState = {};
};

class alignas(FBSIMDAlign) FFLFODSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct LFOGraphRenderData<true>;
  friend struct LFOGraphRenderData<false>;
  std::unique_ptr<FFLFOProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFLFODSPState);
  FFLFODSPState() : processor(std::make_unique<FFLFOProcessor>()) {}
  FBSArray<float, FBFixedBlockSamples> outputAll = {};
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> outputRaw = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFLFOBlockParamState final
{
  friend class FFLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool);
  std::array<TBlock, 1> type = {};
  std::array<TBlock, 1> sync = {};
  std::array<TBlock, 1> seed = {};
  std::array<TBlock, 1> phaseB = {};
  std::array<TBlock, 1> skewAXMode = {};
  std::array<TBlock, 1> skewAYMode = {};
  std::array<TBlock, 1> smoothTime = {};
  std::array<TBlock, 1> smoothBars = {};
  std::array<TBlock, FFLFOBlockCount> steps = {};
  std::array<TBlock, FFLFOBlockCount> opType = {};
  std::array<TBlock, FFLFOBlockCount> rateBars = {};
  std::array<TBlock, FFLFOBlockCount> waveMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFLFOAccParamState final
{
  friend class FFLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool);
  std::array<TAccurate, 1> skewAXAmt = {};
  std::array<TAccurate, 1> skewAYAmt = {};
  std::array<TAccurate, FFLFOBlockCount> min = {};
  std::array<TAccurate, FFLFOBlockCount> max = {};
  std::array<TAccurate, FFLFOBlockCount> rateHz = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFLFOParamState final
{
  friend class FFLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool);
  FFLFOAccParamState<TAccurate> acc = {};
  FFLFOBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOParamState);
};