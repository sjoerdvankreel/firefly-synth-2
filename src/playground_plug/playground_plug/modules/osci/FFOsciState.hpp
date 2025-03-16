#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(sizeof(FBFloatVector)) FFOsciDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OsciGraphRenderData;
  FFOsciProcessor processor = {};
public:
  FBFixedFloatAudioBlock output = {};
  std::array<FBFixedFloatBlock, FFOsciUnisonMaxCount> unisonOutput = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> note = {};
  std::array<TVoiceBlock, 1> unisonCount = {};
  std::array<TVoiceBlock, 1> unisonDetuneHQ = {};
  std::array<TVoiceBlock, 1> unisonOffset = {};
  std::array<TVoiceBlock, 1> unisonOffsetRandom = {};
  std::array<TVoiceBlock, 1> basicSinOn = {};
  std::array<TVoiceBlock, 1> basicSawOn = {};
  std::array<TVoiceBlock, 1> basicTriOn = {};
  std::array<TVoiceBlock, 1> basicSqrOn = {};
  std::array<TVoiceBlock, 1> dsfMode = {};
  std::array<TVoiceBlock, 1> dsfDistance = {};
  std::array<TVoiceBlock, 1> dsfOvertones = {};
  std::array<TVoiceBlock, 1> dsfBandwidth = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TVoiceAcc, 1> cent = {};
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> unisonDetune = {};
  std::array<TVoiceAcc, 1> unisonSpread = {};
  std::array<TVoiceAcc, 1> dsfDecay = {};
  std::array<TVoiceAcc, 1> gLFOToGain = {};
  std::array<TVoiceAcc, 1> basicSqrPW = {};
  std::array<TVoiceAcc, 1> basicSinGain = {};
  std::array<TVoiceAcc, 1> basicSawGain = {};
  std::array<TVoiceAcc, 1> basicTriGain = {};
  std::array<TVoiceAcc, 1> basicSqrGain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  FFOsciAccParamState<TVoiceAcc> acc = {};
  FFOsciBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};