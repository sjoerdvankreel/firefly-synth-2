#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>
#include <playground_base/base/state/FBProcAccParamState.hpp>
#include <playground_base/base/state/FBProcVoiceAccParamState.hpp>
#include <playground_base/base/state/FBProcGlobalAccParamState.hpp>

void
FBProcStatePtrs::InitFrom(
  FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < scalar.values.size(); i++)
    if (!isAcc[i])
      *block[i] = *scalar.values[i];
    else if(isVoice[i])
      voiceAcc[i]->value = *scalar.values[i];
    else
      globalAcc[i]->value = *scalar.values[i];
}