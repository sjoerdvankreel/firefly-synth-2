#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFLFODSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  std::unique_ptr<FFLFOProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFLFODSPState);
  FFLFODSPState() : processor(std::make_unique<FFLFOProcessor>()) {}
  FBSArray<float, FBFixedBlockSamples> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFLFOBlockParamState final
{
  friend class FFLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool);
  std::array<TBlock, 1> on = {};
  std::array<TBlock, 1> sync = {};
  std::array<TBlock, 1> seed = {};
  std::array<TBlock, 1> phase = {};
  std::array<TBlock, 1> steps = {};
  std::array<TBlock, 1> oneShot = {};
  std::array<TBlock, 1> hostSnap = {};
  std::array<TBlock, 1> smoothTime = {};
  std::array<TBlock, 1> smoothBars = {};
  std::array<TBlock, FFLFOBlockCount> stack = {};
  std::array<TBlock, FFLFOBlockCount> opMode = {};
  std::array<TBlock, FFLFOBlockCount> rateBars = {};
  std::array<TBlock, FFLFOBlockCount> waveMode = {};
  std::array<TBlock, FFLFOBlockCount> skewXMode = {};
  std::array<TBlock, FFLFOBlockCount> skewYMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFLFOAccParamState final
{
  friend class FFLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool);
  std::array<TAccurate, FFLFOBlockCount> min = {};
  std::array<TAccurate, FFLFOBlockCount> max = {};
  std::array<TAccurate, FFLFOBlockCount> rateTime = {};
  std::array<TAccurate, FFLFOBlockCount> skewXAmt = {};
  std::array<TAccurate, FFLFOBlockCount> skewYAmt = {};

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