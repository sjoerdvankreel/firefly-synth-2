#pragma once

#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

struct FBProcParamState
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
};