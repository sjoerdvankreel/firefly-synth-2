#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFGLFOExchangeState final
{
  bool active = {};
  int lengthSamples = {};
  int positionSamples = {};
public:
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  friend auto FFSelectGlobalExchangeActiveAddr(auto selectModule);
  friend void FFGLFORenderGraph(FBModuleGraphComponentData* graphData);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOExchangeState);
};

class alignas(sizeof(FBFloatVector)) FFGLFODSPState final
{
  friend class FFPlugProcessor;
  FFGLFOProcessor processor = {};
public:
  FBFixedFloatBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFODSPState);
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
