#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <bool Global>
class FFEchoProcessor;
template <bool Global>
struct EchoGraphRenderData;

class FFEchoGUIState final
{
  friend struct FFGEchoState;
  friend std::unique_ptr<FBStaticModule> FFMakeEchoTopo(bool global);
  std::array<double, 1> tapSelect = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEchoGUIState);
};

template <bool Global>
class FFEchoDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct EchoGraphRenderData<true>;
  friend struct EchoGraphRenderData<false>;
  std::unique_ptr<FFEchoProcessor<Global>> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEchoDSPState);
  FFEchoDSPState() : processor(std::make_unique<FFEchoProcessor<Global>>()) {}
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFEchoBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend class FFEchoProcessor<true>;
  friend class FFEchoProcessor<false>;
  friend std::unique_ptr<FBStaticModule> FFMakeEchoTopo(bool global);
  std::array<TBlock, 1> sync = {};
  std::array<TBlock, 1> order = {};
  std::array<TBlock, 1> tapsOn = {};
  std::array<TBlock, 1> reverbOn = {};
  std::array<TBlock, 1> feedbackOn = {};
  std::array<TBlock, 1> vTargetOrGTarget = {};
  std::array<TBlock, 1> feedbackDelayBars = {};
  std::array<TBlock, 1> voiceFadeTime = {};
  std::array<TBlock, 1> voiceFadeBars = {};
  std::array<TBlock, 1> voiceExtendTime = {};
  std::array<TBlock, 1> voiceExtendBars = {};
  std::array<TBlock, 1> delaySmoothTime = {};
  std::array<TBlock, 1> delaySmoothBars = {};
  std::array<TBlock, FFEchoTapCount> tapOn = {};
  std::array<TBlock, FFEchoTapCount> tapDelayBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEchoBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFEchoAccParamState final
{
  friend class FFEchoProcessor<true>;
  friend class FFEchoProcessor<false>;
  friend std::unique_ptr<FBStaticModule> FFMakeEchoTopo(bool global);
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
  std::array<TAccurate, FFEchoTapCount> tapLPRes = {};
  std::array<TAccurate, FFEchoTapCount> tapHPRes = {};
  std::array<TAccurate, FFEchoTapCount> tapLPFreq = {};
  std::array<TAccurate, FFEchoTapCount> tapHPFreq = {};
  std::array<TAccurate, FFEchoTapCount> tapLevel = {};
  std::array<TAccurate, FFEchoTapCount> tapXOver = {};
  std::array<TAccurate, FFEchoTapCount> tapDelayTime = {};
  std::array<TAccurate, FFEchoTapCount> tapBalance = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEchoAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFEchoParamState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend class FFEchoProcessor<true>;
  friend class FFEchoProcessor<false>;
  friend std::unique_ptr<FBStaticModule> FFMakeEchoTopo(bool global);
  FFEchoAccParamState<TAccurate> acc = {};
  FFEchoBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEchoParamState);
};