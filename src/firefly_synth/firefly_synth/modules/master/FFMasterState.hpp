#pragma once

#include <firefly_synth/modules/master/FFMasterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

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
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TAccurate, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TBlock>
class alignas(alignof(TBlock)) FFMasterBlockParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFMasterParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterAccParamState<TAccurate> acc = {};
  FFMasterBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};