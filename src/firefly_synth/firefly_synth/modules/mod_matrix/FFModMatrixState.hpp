#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>
#include <cstdint>

struct FBStaticModule; 

class alignas(FBSIMDAlign) FFModMatrixDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  std::unique_ptr<FFModMatrixProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFModMatrixDSPState);
  FFModMatrixDSPState() : processor(std::make_unique<FFModMatrixProcessor>()) {}
};

template <class TBlock>
class alignas(alignof(TBlock)) FFModMatrixBlockParamState final
{
  friend class FFModMatrixProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool);
  std::array<TBlock, 1> on = {};
  std::array<TBlock, 1> source = {};
  std::array<TBlock, 1> target = {};
  std::array<TBlock, 1> opType = {};
  std::array<TBlock, 1> bipolar = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFModMatrixAccParamState final
{
  friend class FFModMatrixProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool);
  std::array<TAccurate, 1> amount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFModMatrixParamState final
{
  friend class FFModMatrixProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool);
  FFModMatrixAccParamState<TAccurate> acc = {};
  FFModMatrixBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixParamState);
};