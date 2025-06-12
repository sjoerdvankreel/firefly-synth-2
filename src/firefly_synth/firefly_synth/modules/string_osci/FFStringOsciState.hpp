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
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFStringOsciDSPState);
  FFStringOsciDSPState(): processor(std::make_unique<FFStringOsciProcessor>()) {}
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFStringOsciBlockParamState final
{
  friend class FFStringOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();
  std::array<TBlock, 1> lpOn = {};
  std::array<TBlock, 1> hpOn = {};
  std::array<TBlock, 1> type = {};
  std::array<TBlock, 1> seed = {};
  std::array<TBlock, 1> poles = {};
  std::array<TBlock, 1> uniCount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStringOsciBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFStringOsciAccParamState final
{
  friend class FFStringOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();
  std::array<TAccurate, 1> coarse = {};
  std::array<TAccurate, 1> fine = {};
  std::array<TAccurate, 1> gain = {};
  std::array<TAccurate, 1> uniBlend = {};
  std::array<TAccurate, 1> uniDetune = {};
  std::array<TAccurate, 1> uniSpread = {};
  std::array<TAccurate, 1> x = {};
  std::array<TAccurate, 1> y = {};
  std::array<TAccurate, 1> color = {};
  std::array<TAccurate, 1> excite = {};
  std::array<TAccurate, 1> lpRes = {};
  std::array<TAccurate, 1> hpRes = {};
  std::array<TAccurate, 1> lpFreq = {};
  std::array<TAccurate, 1> hpFreq = {};
  std::array<TAccurate, 1> lpKTrk = {};
  std::array<TAccurate, 1> hpKTrk = {};
  std::array<TAccurate, 1> damp = {};
  std::array<TAccurate, 1> dampKTrk = {};
  std::array<TAccurate, 1> feedback = {};
  std::array<TAccurate, 1> feedbackKTrk = {};
  std::array<TAccurate, 1> trackingKey = {};
  std::array<TAccurate, 1> trackingRange = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStringOsciAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFStringOsciParamState final
{
  friend class FFStringOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();
  FFStringOsciAccParamState<TAccurate> acc = {};
  FFStringOsciBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFStringOsciParamState);
};