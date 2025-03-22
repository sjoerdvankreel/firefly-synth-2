#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(sizeof(FBFloatVector)) FFOsciFMDSPState final
{
  friend class FFVoiceProcessor;
  FFOsciFMProcessor processor = {};
public:
  std::array<FBFixedFloatBlock, FFOsciModSlotCount> outputIndex = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciFMDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciFMBlockParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciFMProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciFMTopo();
  std::array<TVoiceBlock, FFOsciModSlotCount> on = {};
  std::array<TVoiceBlock, FFOsciModSlotCount> throughZero = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciFMBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciFMAccParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciFMProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciFMTopo();
  std::array<TVoiceAcc, FFOsciModSlotCount> index = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciFMAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciFMParamState final
{
  friend class FFOsciProcessor;
  friend class FFOsciFMProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciFMTopo();
  FFOsciFMAccParamState<TVoiceAcc> acc = {};
  FFOsciFMBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciFMParamState);
};