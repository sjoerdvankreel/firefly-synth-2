#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciTopo.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFStringOsciDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct StringOsciGraphRenderData;
  std::unique_ptr<FFStringOsciProcessor> processor = {};
public:
  FFStringOsciDSPState();
  ~FFStringOsciDSPState();
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFStringOsciDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFStringOsciBlockParamState final
{
  friend class FFStringOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> seed = {};
  std::array<TVoiceBlock, 1> poles = {};
  std::array<TVoiceBlock, 1> uniCount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStringOsciBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFStringOsciAccParamState final
{
  friend class FFStringOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();
  std::array<TVoiceAcc, 1> coarse = {};
  std::array<TVoiceAcc, 1> fine = {};
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> uniBlend = {};
  std::array<TVoiceAcc, 1> uniDetune = {};
  std::array<TVoiceAcc, 1> uniSpread = {};
  std::array<TVoiceAcc, 1> x = {};
  std::array<TVoiceAcc, 1> y = {};
  std::array<TVoiceAcc, 1> color = {};
  std::array<TVoiceAcc, 1> excite = {};
  std::array<TVoiceAcc, 1> lpRes = {};
  std::array<TVoiceAcc, 1> hpRes = {};
  std::array<TVoiceAcc, 1> lpFreq = {};
  std::array<TVoiceAcc, 1> hpFreq = {};
  std::array<TVoiceAcc, 1> lpKTrk = {};
  std::array<TVoiceAcc, 1> hpKTrk = {};
  std::array<TVoiceAcc, 1> damp = {};
  std::array<TVoiceAcc, 1> dampKTrk = {};
  std::array<TVoiceAcc, 1> feedback = {};
  std::array<TVoiceAcc, 1> feedbackKTrk = {};
  std::array<TVoiceAcc, 1> trackingKey = {};
  std::array<TVoiceAcc, 1> trackingRange = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStringOsciAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFStringOsciParamState final
{
  friend class FFStringOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();
  FFStringOsciAccParamState<TVoiceAcc> acc = {};
  FFStringOsciBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStringOsciParamState);
};