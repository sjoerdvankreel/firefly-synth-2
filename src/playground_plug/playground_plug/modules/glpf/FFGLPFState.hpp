#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>
#include <playground_plug/modules/glpf/FFGLPFProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(sizeof(FBFloatVector)) FFGLPFDSPState final
{
  friend class FFPlugProcessor;
  FFGLPFProcessor processor = {};
public:
  FBFixedAudioBlock input = {};
  FBFixedAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLPFDSPState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGLPFBlockParamState final
{
  friend class FFGLPFProcessor;
  friend FBStaticModule FFMakeGLPFTopo();
  std::array<TGlobalBlock, 1> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLPFBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLPFAccParamState final
{
  friend class FFGLPFProcessor;
  friend FBStaticModule FFMakeGLPFTopo();
  std::array<TGlobalAcc, 1> res = {};
  std::array<TGlobalAcc, 1> freq = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLPFAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLPFParamState final
{
  friend class FFGLPFProcessor;
  friend FBStaticModule FFMakeGLPFTopo();
  FFGLPFAccParamState<TGlobalAcc> acc = {};
  FFGLPFBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLPFParamState);
};