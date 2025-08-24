#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoProcessor.hpp>

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
  friend struct GEchoGraphRenderData;
  std::unique_ptr<FFGEchoProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGEchoDSPState);
  FFGEchoDSPState() : processor(std::make_unique<FFGEchoProcessor>()) {}
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGEchoBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFGEchoProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  std::array<TBlock, 1> sync = {};
  std::array<TBlock, 1> order = {};
  std::array<TBlock, 1> target = {};
  std::array<TBlock, 1> tapsOn = {};
  std::array<TBlock, 1> reverbOn = {};
  std::array<TBlock, 1> feedbackOn = {};
  std::array<TBlock, 1> feedbackDelayBars = {};
  std::array<TBlock, 1> delaySmoothTime = {};
  std::array<TBlock, 1> delaySmoothBars = {};
  std::array<TBlock, FFGEchoTapCount> tapOn = {};
  std::array<TBlock, FFGEchoTapCount> tapDelayBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGEchoAccParamState final
{
  friend class FFGEchoProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  std::array<TAccurate, 1> gain = {};
  std::array<TAccurate, 1> tapsMix = {};
  std::array<TAccurate, 1> reverbMix = {};
  std::array<TAccurate, 1> reverbAPF = {};
  std::array<TAccurate, 1> reverbSize = {};
  std::array<TAccurate, 1> reverbDamp = {};
  std::array<TAccurate, 1> reverbXOver = {};
  std::array<TAccurate, 1> reverbLPRes = {};
  std::array<TAccurate, 1> reverbHPRes = {};
  std::array<TAccurate, 1> reverbLPFreq = {};
  std::array<TAccurate, 1> reverbHPFreq = {};
  std::array<TAccurate, 1> feedbackMix = {};
  std::array<TAccurate, 1> feedbackAmount = {};
  std::array<TAccurate, 1> feedbackXOver = {};
  std::array<TAccurate, 1> feedbackDelayTime = {};
  std::array<TAccurate, 1> feedbackLPFreq = {};
  std::array<TAccurate, 1> feedbackLPRes = {};
  std::array<TAccurate, 1> feedbackHPFreq = {};
  std::array<TAccurate, 1> feedbackHPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapLPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapHPRes = {};
  std::array<TAccurate, FFGEchoTapCount> tapLPFreq = {};
  std::array<TAccurate, FFGEchoTapCount> tapHPFreq = {};
  std::array<TAccurate, FFGEchoTapCount> tapLevel = {};
  std::array<TAccurate, FFGEchoTapCount> tapXOver = {};
  std::array<TAccurate, FFGEchoTapCount> tapDelayTime = {};
  std::array<TAccurate, FFGEchoTapCount> tapBalance = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFGEchoParamState final
{
  friend class FFPlugProcessor;
  friend class FFGEchoProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();
  FFGEchoAccParamState<TAccurate> acc = {};
  FFGEchoBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoParamState);
};