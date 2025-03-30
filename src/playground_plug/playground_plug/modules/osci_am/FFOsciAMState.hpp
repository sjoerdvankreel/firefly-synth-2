#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBFixedBlockAlign) FFOsciAMDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OsciGraphRenderData;
  FFOsciAMProcessor processor = {};
public:
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputMix = {};
  std::array<FBFixedFloatArray, FFOsciModSlotCount> outputRing = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciAMBlockParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciAMProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
  std::array<TVoiceBlock, FFOsciModSlotCount> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAMAccParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciAMProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
  std::array<TVoiceAcc, FFOsciModSlotCount> mix = {};
  std::array<TVoiceAcc, FFOsciModSlotCount> ring = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAMParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciAMProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
  FFOsciAMAccParamState<TVoiceAcc> acc = {};
  FFOsciAMBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMParamState);
};