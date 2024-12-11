#include <playground_base_clap/FBCLAPPlugin.hpp>

bool 
FBCLAPPlugin::implementsAudioPorts() const noexcept
{
  return true;
}

uint32_t 
FBCLAPPlugin::audioPortsCount(bool isInput) const noexcept
{
  return isInput ? 0 : 1;
}

bool
FBCLAPPlugin::audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info* info) const noexcept 
{
  if (isInput)
    return false;
  if (index != 0)
    return false;

  info->id = 0;
  info->name[0] = 0;
  info->channel_count = 2;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = CLAP_INVALID_ID;
  info->flags = CLAP_AUDIO_PORT_IS_MAIN;
  return true;
}