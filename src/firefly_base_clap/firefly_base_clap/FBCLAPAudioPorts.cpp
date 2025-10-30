#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

bool 
FBCLAPPlugin::implementsAudioPorts() const noexcept
{
  return true;
}

uint32_t 
FBCLAPPlugin::audioPortsCount(bool /*isInput*/) const noexcept
{
  return 1;
}

bool
FBCLAPPlugin::audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info* info) const noexcept 
{
  if (index != 0)
    return false;

  info->id = 0;
  info->channel_count = 2;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = CLAP_INVALID_ID;

  std::fill(info->name, info->name + sizeof(info->name), 0);
  if (!isInput)
  {
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    std::strcpy(info->name, "Stereo Out");
  }
  else
  {
    // The bustype is important to bitwig.
    // With main, it wont accept sidechaining.
    if (_topo->static_->meta.isFx)
    {
      info->flags = CLAP_AUDIO_PORT_IS_MAIN;
      std::strcpy(info->name, "Stereo In");
    }
    else
    {
      info->flags = 0;
      std::strcpy(info->name, "Sidechain");
    }
  }

  return true;
}