#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(sizeof(FBFloatVector)) FFGFilterDSPState final
{
  friend class FFPlugProcessor;
  FFGFilterProcessor processor = {};
public:
  FBFixedAudioBlock input = {};
  FBFixedAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterDSPState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGFilterBlockParamState final
{
  friend class FFGFilterProcessor;
  friend FBStaticModule FFMakeGFilterTopo();
  std::array<TGlobalBlock, 1> on = {};
  std::array<TGlobalBlock, 1> type = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGFilterAccParamState final
{
  friend class FFGFilterProcessor;
  friend FBStaticModule FFMakeGFilterTopo();
  std::array<TGlobalAcc, 1> res = {};
  std::array<TGlobalAcc, 1> freq = {};
  std::array<TGlobalAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGFilterParamState final
{
  friend class FFGFilterProcessor;
  friend FBStaticModule FFMakeGFilterTopo();
  FFGFilterAccParamState<TGlobalAcc> acc = {};
  FFGFilterBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterParamState);
};