#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFGEchoProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFGEchoGUIState final
{
  friend struct FFGEchoState;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  std::array<double, 1> tapSelect = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoGUIState);
};

class FFGEchoDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFGEchoProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGEchoDSPState);
  FFGEchoDSPState() : processor(std::make_unique<FFGEchoProcessor>()) {}
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGEchoBlockParamState final
{
  friend class FFGEchoProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  std::array<TBlock, 1> on = {};
  std::array<TBlock, 1> sync = {};
  std::array<TBlock, 1> target = {};
  std::array<TBlock, FFGEchoTapCount> tapOn = {};
  std::array<TBlock, FFGEchoTapCount> tapLPOn = {};
  std::array<TBlock, FFGEchoTapCount> tapHPOn = {};
  std::array<TBlock, FFGEchoTapCount> tapFBLPOn = {};
  std::array<TBlock, FFGEchoTapCount> tapFBHPOn = {};
  std::array<TBlock, FFGEchoTapCount> tapDelayTime = {};
  std::array<TBlock, FFGEchoTapCount> tapDelayBars = {};
  std::array<TBlock, FFGEchoTapCount> tapLengthBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGEchoAccParamState final
{
  friend class FFGEchoProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  std::array<TAccurate, 1> mix = {};
  std::array<TAccurate, FFGEchoTapCount> tapLPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapHPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapLPFreq = {};
  std::array<TAccurate, FFGEchoTapCount> tapHPFreq = {};
  std::array<TAccurate, FFGEchoTapCount> tapFBLPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapFBHPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapFBLPFreq = {};
  std::array<TAccurate, FFGEchoTapCount> tapFBHPFreq = {};
  std::array<TAccurate, FFGEchoTapCount> tapLevel = {};
  std::array<TAccurate, FFGEchoTapCount> tapBalance = {};
  std::array<TAccurate, FFGEchoTapCount> tapFeedback = {};
  std::array<TAccurate, FFGEchoTapCount> tapCrossOver = {};
  std::array<TAccurate, FFGEchoTapCount> tapFBCrossOver = {};
  std::array<TAccurate, FFGEchoTapCount> tapLengthTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFGEchoParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  FFGEchoAccParamState<TAccurate> acc = {};
  FFGEchoBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoParamState);
};