#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>

void
FBScalarStatePtrs::InitFrom(FBProcStatePtrs const& proc)
{
  for (int i = 0; i < values.size(); i++)
    if (proc.isAcc[i])
      if (proc.isVoice[i])
        *values[i] = proc.voiceAcc[i]->value;
      else
        *values[i] = proc.globalAcc[i]->value;
    else 
      if (proc.isVoice[i])
        *values[i] = proc.voiceBlock[i]->value;
      else
        *values[i] = *proc.globalBlock[i];
}

void 
FBScalarStatePtrs::InitFrom(FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < values.size(); i++)
    *values[i] = *scalar.values[i];
}