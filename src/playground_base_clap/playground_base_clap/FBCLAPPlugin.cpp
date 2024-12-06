#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <clap/helpers/plugin.hxx>

FBCLAPPlugin::
FBCLAPPlugin(
  FBStaticTopo const& topo,
  clap_plugin_descriptor const* desc, 
  clap_host const* host):
Plugin(desc, host),
_topo(std::make_unique<FBRuntimeTopo>(topo)) {}

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

// TODO move

uint32_t 
FBCLAPPlugin::paramsCount() const noexcept
{

}

bool 
FBCLAPPlugin::paramsValue(clap_id paramId, double* value) noexcept
{

}

bool 
FBCLAPPlugin::paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept
{

}

bool 
FBCLAPPlugin::paramsTextToValue(clap_id paramId, const char* display, double* value) noexcept
{

}

void 
FBCLAPPlugin::paramsFlush(const clap_input_events* in, const clap_output_events* out) noexcept
{
}

bool 
FBCLAPPlugin::paramsValueToText(clap_id paramId, double value, char* display, uint32_t size) noexcept
{
}
