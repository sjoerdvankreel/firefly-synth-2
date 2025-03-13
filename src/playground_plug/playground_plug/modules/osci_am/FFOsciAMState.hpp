#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciAMBlockParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
  std::array<TVoiceBlock, FFOsciAMSlotCount> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAMAccParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
  std::array<TVoiceAcc, FFOsciAMSlotCount> mix = {};
  std::array<TVoiceAcc, FFOsciAMSlotCount> ring = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAMParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
  FFOsciAMAccParamState<TVoiceAcc> acc = {};
  FFOsciAMBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMParamState);
};