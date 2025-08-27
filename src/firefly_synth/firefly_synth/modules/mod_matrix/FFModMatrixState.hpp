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

template <bool Global>
class FFModMatrixProcessor;

template <bool Global>
class alignas(FBSIMDAlign) FFModMatrixDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend class FFModMatrixProcessor<Global>;
  friend class FFModMatrixProcessor<!Global>; // clang
  static inline int constexpr MaxSlotCount = FFModMatrixTraits<Global>::MaxSlotCount;

  std::unique_ptr<FFModMatrixProcessor<Global>> processor = {};
  FBSArray2<float, FBFixedBlockSamples, MaxSlotCount> modulatedCV = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFModMatrixDSPState);
  FFModMatrixDSPState() : processor(std::make_unique<FFModMatrixProcessor<Global>>()) {}
};

template <class TBlock, bool Global>
class alignas(alignof(TBlock)) FFModMatrixBlockParamState final
{
  friend class FFModMatrixProcessor<Global>;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool, FFStaticTopo const*);
  static inline int constexpr MaxSlotCount = FFModMatrixTraits<Global>::MaxSlotCount;

  std::array<TBlock, 1> slots = {};
  std::array<TBlock, MaxSlotCount> scale = {};
  std::array<TBlock, MaxSlotCount> opType = {};
  std::array<TBlock, MaxSlotCount> source = {};
  std::array<TBlock, MaxSlotCount> target = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixBlockParamState);
};

template <class TAccurate, bool Global>
class alignas(alignof(TAccurate)) FFModMatrixAccParamState final
{
  friend class FFModMatrixProcessor<Global>;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool, FFStaticTopo const*);
  static inline int constexpr MaxSlotCount = FFModMatrixTraits<Global>::MaxSlotCount;

  std::array<TAccurate, MaxSlotCount> amount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixAccParamState);
};

template <class TBlock, class TAccurate, bool Global>
class alignas(alignof(TAccurate)) FFModMatrixParamState final
{
  friend class FFModMatrixProcessor<Global>;
  friend std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool, FFStaticTopo const*);
  FFModMatrixAccParamState<TAccurate, Global> acc = {};
  FFModMatrixBlockParamState<TBlock, Global> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixParamState);
};