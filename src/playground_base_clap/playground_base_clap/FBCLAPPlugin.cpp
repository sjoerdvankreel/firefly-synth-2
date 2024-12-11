#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

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

void
FBCLAPPlugin::deactivate() noexcept
{
  _hostProcessor.reset();
}

bool
FBCLAPPlugin::activate(double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept 
{
  float fSampleRate = static_cast<float>(sampleRate);
  _procStatePtrs.Init(fSampleRate);
  auto plug = MakePlugProcessor(_topo->static_, _procState, fSampleRate);
  _hostProcessor.reset(new FBHostProcessor(std::move(plug), &_procStatePtrs, fSampleRate));
  return true;
}

clap_process_status 
FBCLAPPlugin::process(const clap_process* process) noexcept 
{
  return CLAP_PROCESS_SLEEP;
}