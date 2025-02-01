#include <playground_base/base/state/FBProcParamState.hpp>

void
FBProcParamState::InitProcessing(float value)
{
  Value(value);
  switch (Type())
  {
  case FBProcParamType::VoiceAcc: VoiceAcc().InitProcessing(value); break;
  case FBProcParamType::GlobalAcc: GlobalAcc().InitProcessing(value); break;
  default: break;
  }
}

void
FBProcParamState::SetSmoothingCoeffs(int sampleCount)
{
  switch (Type())
  {
  case FBProcParamType::VoiceBlock: break;
  case FBProcParamType::GlobalBlock: break;
  case FBProcParamType::VoiceAcc: VoiceAcc().SetSmoothingCoeffs(sampleCount); break;
  case FBProcParamType::GlobalAcc: GlobalAcc().SetSmoothingCoeffs(sampleCount); break;
  default: assert(false);
  }
}