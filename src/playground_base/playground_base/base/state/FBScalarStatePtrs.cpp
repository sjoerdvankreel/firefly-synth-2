#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>
#include <playground_base/base/state/FBProcAccParamState.hpp>
#include <playground_base/base/state/FBProcVoiceAccParamState.hpp>
#include <playground_base/base/state/FBProcGlobalAccParamState.hpp>

void
FBScalarStatePtrs::InitFrom(FBProcStatePtrs const& proc)
{
  for (int i = 0; i < values.size(); i++)
    if (!proc.isAcc[i])
      *values[i] = *proc.block[i];
    else if (proc.isVoice[i])
      *values[i] = proc.voiceAcc[i]->value;
    else
      *values[i] = proc.globalAcc[i]->value;
}

void 
FBScalarStatePtrs::InitFrom(FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < values.size(); i++)
    *values[i] = *scalar.values[i];
}