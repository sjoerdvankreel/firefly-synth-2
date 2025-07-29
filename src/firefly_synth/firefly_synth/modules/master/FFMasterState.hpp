#pragma once

#include <firefly_synth/modules/master/FFMasterTopo.hpp>
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
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFMasterDSPState);
  FFMasterDSPState() : processor(std::make_unique<FFMasterProcessor>()) {}
  FBSArray2<float, FBFixedBlockSamples, FFMasterAuxCount> outputAux = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFMasterBlockParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TAccurate, FFMasterAuxCount> aux = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFMasterParamState final
{
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterBlockParamState<TBlock> block = {};
  FFMasterAccParamState<TAccurate> acc = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};