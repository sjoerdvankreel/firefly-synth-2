#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

bool 
FBCLAPPlugin::implementsVoiceInfo() const noexcept
{
  return true;
}

bool 
FBCLAPPlugin::voiceInfoGet(clap_voice_info* info) noexcept
{
  info->voice_count = FBMaxVoices;
  info->voice_capacity = FBMaxVoices;
  info->flags = CLAP_VOICE_INFO_SUPPORTS_OVERLAPPING_NOTES;
  return true;
}