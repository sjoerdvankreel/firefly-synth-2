#include <firefly_base/base/state/proc/FBProcParamState.hpp>

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
FBProcParamState::InitProcessing(int voice, float value)
{
  Value(value);
  switch (Type())
  {
  case FBProcParamType::VoiceBlock: VoiceBlock().BeginVoice(voice); break;
  case FBProcParamType::VoiceAcc: VoiceAcc().InitProcessing(voice, value); break;
  default: FB_ASSERT(false); break;
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
  default: FB_ASSERT(false);
  }
}