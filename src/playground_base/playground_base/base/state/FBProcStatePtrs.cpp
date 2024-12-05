#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void
FBProcStatePtrs::InitFrom(
  FBScalarStatePtrs const& scalar)
{
  single.InitFrom(isAcc, scalar);
  for (int v = 0; v < FB_MAX_VOICES; v++)
    voice[v].InitFrom(isAcc, scalar);
}

void 
FBProcSingleStatePtrs::InitFrom(
  std::vector<bool> const& isAcc,
  FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < scalar.all.size(); i++)
    if (isAcc[i])
      acc[i]->Reset(*scalar.all[i]);
    else
      *block[i] = *scalar.all[i];
}