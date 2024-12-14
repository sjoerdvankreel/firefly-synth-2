#pragma once

#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedVectorBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBVectorByteCount) FFGLFODSPState final
{
  friend class FFPlugProcessor;
  FFGLFOProcessor processor = {};
public:
  FBFixedVectorBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFODSPState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGLFOBlockParamState final
{
  friend class FFGLFOProcessor;
  friend FBStaticModule FFMakeGLFOTopo();
  std::array<TGlobalBlock, 1> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOAccParamState final
{
  friend class FFGLFOProcessor;
  friend FBStaticModule FFMakeGLFOTopo();
  std::array<TGlobalAcc, 1> rate = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOParamState final
{
  friend class FFGLFOProcessor;
  friend FBStaticModule FFMakeGLFOTopo();
  FFGLFOAccParamState<TGlobalAcc> acc = {};
  FFGLFOBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};
