#pragma once

#include <firefly_synth/modules/gfilter/FFGFilterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFGFilterDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFGFilterProcessor> processor = {};
public:
  FFGFilterDSPState();
  ~FFGFilterDSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGFilterDSPState);
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGFilterBlockParamState final
{
  friend class FFGFilterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();
  std::array<TBlock, 1> on = {};
  std::array<TBlock, 1> mode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGFilterAccParamState final
{
  friend class FFGFilterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();
  std::array<TAccurate, 1> res = {};
  std::array<TAccurate, 1> freq = {};
  std::array<TAccurate, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFGFilterParamState final
{
  friend class FFGFilterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();
  FFGFilterAccParamState<TAccurate> acc = {};
  FFGFilterBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterParamState);
};