#include <playground_base/base/state/FBProcParamState.hpp>

void
FBProcParamState::Reset(float val)
{
  pos = 0;
  scalar = val;
  smooth.Reset(val);
  rampedCV.Fill(0, FBFixedCVBlock::Count(), val);
  smoothedCV.Fill(0, FBFixedCVBlock::Count(), val);
}