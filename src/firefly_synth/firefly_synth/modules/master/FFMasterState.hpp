#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFMasterBlockParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFMasterParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};