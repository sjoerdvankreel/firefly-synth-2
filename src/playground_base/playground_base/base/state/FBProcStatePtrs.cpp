#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void 
FBProcSingleStatePtrs::InitFrom(
  bool voice,
  std::vector<bool> const& isAcc,
  std::vector<bool> const& isVoice,
  FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < scalar.all.size(); i++)
    if(!voice || isVoice[i])
      if (isAcc[i])
        acc[i]->Reset(*scalar.all[i]);
      else
        *block[i] = *scalar.all[i];
}

void
FBProcStatePtrs::InitFrom(
  FBScalarStatePtrs const& scalar)
{
  single.InitFrom(false, isAcc, isVoice, scalar);
  for (int i = 0; i < scalar.all.size(); i++)
    if (isVoice[i])
      for (int v = 0; v < FB_MAX_VOICES; v++)
        voice[v].InitFrom(true, isAcc, isVoice, scalar);
}