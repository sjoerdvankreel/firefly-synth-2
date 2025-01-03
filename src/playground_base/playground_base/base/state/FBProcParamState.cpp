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
FBProcParamState::SetSmoothingCoeffs(float sampleRate, float durationSecs)
{
  switch (Type())
  {
  case FBProcParamType::VoiceBlock: break;
  case FBProcParamType::GlobalBlock: break;
  case FBProcParamType::VoiceAcc: VoiceAcc().SetSmoothingCoeffs(sampleRate, durationSecs); break;
  case FBProcParamType::GlobalAcc: GlobalAcc().SetSmoothingCoeffs(sampleRate, durationSecs); break;
  default: assert(false);
  }
}