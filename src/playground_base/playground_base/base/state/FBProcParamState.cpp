#include <playground_base/base/state/FBProcParamState.hpp>

void
FBProcParamState::Init(float sampleRate)
{
  switch (Type())
  {
  case FBProcParamType::VoiceBlock: break;
  case FBProcParamType::GlobalBlock: break;
  case FBProcParamType::VoiceAcc: VoiceAcc().Init(sampleRate); break;
  case FBProcParamType::GlobalAcc: GlobalAcc().Init(sampleRate); break;
  default: assert(false);
  }
}