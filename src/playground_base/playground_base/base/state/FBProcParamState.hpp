#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

struct FBProcParamState final
{
  friend class FBHostProcessor;
  friend class FBRampProcessor;
  friend class FBSmoothProcessor;

private:
  int pos = 0;
  FBOnePoleFilter smooth = {};
  FBFixedCVBlock rampedCV = {};

public:
  FBFixedCVBlock smoothedCV = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcParamState);
};