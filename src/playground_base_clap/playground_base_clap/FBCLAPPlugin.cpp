#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <clap/helpers/plugin.hxx>

FBCLAPPlugin::
FBCLAPPlugin(clap_plugin_descriptor const* desc, clap_host const* host):
Plugin(desc, host) {}

bool 
FBCLAPPlugin::isValidParamId(clap_id paramId) const noexcept
{
  return false;
}

int32_t 
FBCLAPPlugin::getParamIndexForParamId(clap_id paramId) const noexcept
{
  return 0;
}

bool 
FBCLAPPlugin::getParamInfoForParamId(clap_id paramId, clap_param_info* info) const noexcept
{
  return false;
}
