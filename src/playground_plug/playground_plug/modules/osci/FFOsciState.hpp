#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBVectorByteCount) FFOsciDSPState final
{
  friend class FFPlugProcessor;
  FFOsciProcessor processor = {};
public:
  FBFixedAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend FBStaticModule FFMakeOsciTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend FBStaticModule FFMakeOsciTopo();
  std::array<TVoiceAcc, 1> pitch = {};
  std::array<TVoiceAcc, 1> glfoToGain = {};
  std::array<TVoiceAcc, FFOsciGainCount> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend FBStaticModule FFMakeOsciTopo();
  FFOsciAccParamState<TVoiceAcc> acc = {};
  FFOsciBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};