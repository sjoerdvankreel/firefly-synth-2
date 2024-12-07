#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>

void
FBProcStatePtrs::InitFrom(
  FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < scalar.values.size(); i++)
    if (isAcc[i])
      if (isVoice[i])
        voiceAcc[i]->value = *scalar.values[i];
      else
        globalAcc[i]->value = *scalar.values[i];
    else
      if (isVoice[i])
        *voiceBlock[i].value = *scalar.values[i];
      else
        *globalBlock[i] = *scalar.values[i];
}