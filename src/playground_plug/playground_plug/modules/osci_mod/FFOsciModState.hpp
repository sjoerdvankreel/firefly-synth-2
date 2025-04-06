#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBFixedBlockAlign) FFOsciModDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OscisGraphRenderData;
  FFOsciModProcessor processor = {};
public:
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputAMMix = {};
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputFMIndex = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciModBlockParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TVoiceBlock, 1> oversampling4X = {};
  std::array<TVoiceBlock, FFOsciModSlotCount> amMode = {};
  std::array<TVoiceBlock, FFOsciModSlotCount> fmMode = {};
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