#pragma once

#include <firefly_synth/modules/output/FFOutputProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFOutputDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFOutputProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOutputDSPState);
  FFOutputDSPState(): processor(std::make_unique<FFOutputProcessor>()) {}
};

template <class TBlock>
class alignas(alignof(TBlock)) FFOutputBlockParamState final
{
  friend class FFOutputProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
  std::array<TBlock, 1> voices = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputBlockParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFOutputParamState final
{
  friend class FFOutputProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
  FFOutputBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputParamState);
};