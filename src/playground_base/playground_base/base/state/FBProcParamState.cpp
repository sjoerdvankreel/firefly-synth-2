#include <playground_base/base/state/FBProcParamState.hpp>

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