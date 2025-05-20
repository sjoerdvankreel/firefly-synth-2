#pragma once

#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFMasterDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFMasterProcessor> processor = {};
public:
  FFMasterDSPState();
  ~FFMasterDSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFMasterDSPState);
  FBArray2<float, FBFixedBlockSamples, 2> input = {};
  FBArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TGlobalAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFMasterBlockParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TGlobalBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterAccParamState<TGlobalAcc> acc = {};
  FFMasterBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};