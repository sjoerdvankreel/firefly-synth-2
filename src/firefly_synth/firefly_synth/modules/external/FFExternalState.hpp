#pragma once

#include <firefly_synth/modules/external/FFExternalTopo.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFExternalBlockParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeExternalTopo();
  std::array<TBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFExternalBlockParamState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFExternalParamState final
{
  friend class FFExternalProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeExternalTopo();
  FFExternalBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFExternalParamState);
};