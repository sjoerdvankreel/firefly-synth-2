#pragma once

#include <firefly_synth/modules/glfo/FFGLFOProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/exchange/FBModuleExchangeState.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFGLFODSPState final
{
  friend class FFPlugProcessor;
  friend struct GLFOGraphRenderData;
  std::unique_ptr<FFGLFOProcessor> processor = {};
public:
  FFGLFODSPState();
  ~FFGLFODSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGLFODSPState);
  FBSArray<float, FBFixedBlockSamples> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGLFOBlockParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  std::array<TBlock, 1> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGLFOAccParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  std::array<TAccurate, 1> rate = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFGLFOParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGLFOTopo();
  FFGLFOAccParamState<TAccurate> acc = {};
  FFGLFOBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};
