#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/base/state/exchange/FBModuleProcExchangeState.hpp>

#include <array>
#include <memory>

struct FBStaticModule;
class FFGLFOProcessor;

struct FFGLFOExchangeState final:
public FBModuleProcExchangeState
{
  float lastOutput = {};
};

class alignas(FBFixedBlockAlign) FFGLFODSPState final
{
  friend class FFPlugProcessor;
  friend struct GLFOGraphRenderData;
  std::unique_ptr<FFGLFOProcessor> processor = {};
public:
  FFGLFODSPState();
  ~FFGLFODSPState();
  FBFixedFloatArray output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGLFODSPState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGLFOBlockParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  std::array<TGlobalBlock, 1> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOAccParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  std::array<TGlobalAcc, 1> rate = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  FFGLFOAccParamState<TGlobalAcc> acc = {};
  FFGLFOBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};
