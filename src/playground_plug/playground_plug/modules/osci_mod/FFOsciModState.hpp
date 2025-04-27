#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>

#include <array>
#include <memory>

struct FBStaticModule;
class FFOsciModProcessor;

class alignas(FBSIMDAlign) FFOsciModDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OscisGraphRenderData;
  std::unique_ptr<FFOsciModProcessor> processor = {};
public:
  FFOsciModDSPState();
  ~FFOsciModDSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciModDSPState);
  FBSIMDArray2<float, FBFixedBlockSamples, FFOsciModSlotCount> outputAMMix = {};
  FBSIMDArray2<float, FBFixedBlockSamples, FFOsciModSlotCount> outputFMIndex = {};
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciModBlockParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TVoiceBlock, 1> expoFM = {};
  std::array<TVoiceBlock, 1> oversampling = {};
  std::array<TVoiceBlock, FFOsciModSlotCount> fmOn = {};
  std::array<TVoiceBlock, FFOsciModSlotCount> amMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciModAccParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TVoiceAcc, FFOsciModSlotCount> amMix = {};
  std::array<TVoiceAcc, FFOsciModSlotCount> fmIndex = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciModParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  FFOsciModAccParamState<TVoiceAcc> acc = {};
  FFOsciModBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModParamState);
};