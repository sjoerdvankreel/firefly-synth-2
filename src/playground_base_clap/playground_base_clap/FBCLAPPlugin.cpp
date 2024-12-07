#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <clap/helpers/plugin.hxx>

FBCLAPPlugin::
~FBCLAPPlugin()
{ 
  _topo->static_.freeProcState(_procState); 
}

FBCLAPPlugin::
FBCLAPPlugin(
  FBStaticTopo const& topo,
  clap_plugin_descriptor const* desc, 
  clap_host const* host):
Plugin(desc, host),
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_procState(topo.allocProcState()),
_procStatePtrs(_topo->MakeProcStatePtrs(_procState)) {}

bool 
FBCLAPPlugin::isValidParamId(
  clap_id paramId) const noexcept
{
  return _topo->tagToParam.find(paramId) != _topo->tagToParam.end();
}

int32_t 
FBCLAPPlugin::getParamIndexForParamId(
  clap_id paramId) const noexcept
{
  auto iter = _topo->tagToParam.find(paramId);
  return iter == _topo->tagToParam.end() ? -1 : iter->second;
}

bool 
FBCLAPPlugin::getParamInfoForParamId(
  clap_id paramId, clap_param_info* info) const noexcept
{
  auto iter = _topo->tagToParam.find(paramId);
  if (iter == _topo->tagToParam.end())
    return false;
  return paramsInfo(iter->second, info);
}