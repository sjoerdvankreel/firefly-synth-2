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
  friend struct OsciGraphRenderData;
  FFOsciModProcessor processor = {};
public:
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputAM = {};
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputRM = {};
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputFM = {};
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputTZFM = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciModBlockParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TVoiceBlock, FFOsciModSlotCount> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciModAccParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciModProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();
  std::array<TVoiceAcc, FFOsciModSlotCount> am = {};
  std::array<TVoiceAcc, FFOsciModSlotCount> rm = {};
  std::array<TVoiceAcc, FFOsciModSlotCount> fm = {};
  std::array<TVoiceAcc, FFOsciModSlotCount> tzfm = {};
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