#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>
#include <cstdint>

struct FFStaticTopo;
struct FBStaticModule;

class FFModMatrixDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  std::unique_ptr<FFModMatrixProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFModMatrixDSPState);
  FFModMatrixDSPState() : processor(std::make_unique<FFModMatrixProcessor>()) {}
};

template <class TBlock, int SlotCount>
class alignas(alignof(TBlock)) FFModMatrixBlockParamState final
{
  friend class FFModMatrixProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool, FFStaticTopo const*);
  std::array<TBlock, SlotCount> scale = {};
  std::array<TBlock, SlotCount> opType = {};
  std::array<TBlock, SlotCount> source = {};
  std::array<TBlock, SlotCount> target = {};
  std::array<TBlock, SlotCount> bipolar = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixBlockParamState);
};

template <class TAccurate, int SlotCount>
class alignas(alignof(TAccurate)) FFModMatrixAccParamState final
{
  friend class FFModMatrixProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool, FFStaticTopo const*);
  std::array<TAccurate, SlotCount> amount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixAccParamState);
};

template <class TBlock, class TAccurate, int SlotCount>
class alignas(alignof(TAccurate)) FFModMatrixParamState final
{
  friend class FFModMatrixProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool, FFStaticTopo const*);
  FFModMatrixAccParamState<TAccurate, SlotCount> acc = {};
  FFModMatrixBlockParamState<TBlock, SlotCount> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixParamState);
};