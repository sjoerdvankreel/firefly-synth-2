#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

struct FBProcParamState final
{
  friend class FBHostProcessor;
  friend class FBRampProcessor;
  friend class FBSmoothProcessor;

  friend struct FBScalarStatePtrs;
  friend struct FBProcSingleStatePtrs;

private:
  int pos = 0;
  FBOnePoleFilter smooth = {};
  FBFixedCVBlock rampedCV = {};
  
  void Reset(float val);

public:
  float scalar = 0.0f;
  FBFixedCVBlock smoothedCV = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcParamState);
};