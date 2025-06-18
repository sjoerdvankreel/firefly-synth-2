#pragma once

#include <firefly_synth/modules/osci_mod/FFOsciModTopo.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

inline int constexpr FFOsciOversampleFactor = 2;
inline int constexpr FFOsciOversampleTimes = 1 << FFOsciOversampleFactor;
inline int constexpr FFOsciFixedBlockOversamples = FBFixedBlockSamples * FFOsciOversampleTimes;

class alignas(FBSIMDAlign) FFOsciModDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OsciGraphRenderData;
  std::unique_ptr<FFOsciModProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciModDSPState);
  FFOsciModDSPState(): processor(std::make_unique<FFOsciModProcessor>()) {}
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciModSlotCount> outputAMMix = {};
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciModSlotCount> outputFMIndex = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFOsciModBlockParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TBlock, 1> expoFM = {};
  std::array<TBlock, 1> oversample = {};
  std::array<TBlock, FFOsciModSlotCount> fmOn = {};
  std::array<TBlock, FFOsciModSlotCount> amMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFOsciModAccParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TAccurate, FFOsciModSlotCount> amMix = {};
  std::array<TAccurate, FFOsciModSlotCount> fmIndex = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFOsciModParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  FFOsciModAccParamState<TAccurate> acc = {};
  FFOsciModBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModParamState);
};