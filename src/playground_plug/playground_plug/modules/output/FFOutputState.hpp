#pragma once

#include <playground_plug/modules/output/FFOutputProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFOutputDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFOutputProcessor> processor = {};
public:
  FFOutputDSPState();
  ~FFOutputDSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOutputDSPState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFOutputBlockParamState final
{
  friend class FFOutputProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
  std::array<TGlobalBlock, 1> voices = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputBlockParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFOutputParamState final
{
  friend class FFOutputProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
  FFOutputBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputParamState);
};