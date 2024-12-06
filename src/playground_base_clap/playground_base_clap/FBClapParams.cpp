#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>

uint32_t 
FBCLAPPlugin::paramsCount() const noexcept
{
  return static_cast<uint32_t>(_topo->params.size());
}

bool 
FBCLAPPlugin::paramsValue(clap_id paramId, double* value) noexcept
{
  int32_t index = getParamIndexForParamId(paramId);
  if (index == -1)
    return false;
  if (_statePtrs.isAcc[index])
    return _statePtrs.single.acc[index]->scalar;
  return *_statePtrs.single.block[index];
}

bool 
FBCLAPPlugin::paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept
{
  if (paramIndex >= _topo->params.size())
    return false;

  auto const& param = _topo->params[paramIndex];
  auto const& module = _topo->modules[param.]

  info->id = param.tag;
  info->max_value = 1.0;
  info->min_value = 0.0;
  info->cookie = nullptr;
  info->default_value = param.static_.defaultNormalized;
  std::fill(info->name, info->name + sizeof(info->name), 0);
  strncpy(info->name, param.shortName.c_str(), sizeof(info->name) - 1);
  std::fill(info->module, info->module + sizeof(info->module), 0);
  strncpy(info->module, param.mo.c_str(), sizeof(info->name) - 1);
  std::copy(info->name, info->name + param.shortName.size(), )
  info->name param.shortName
  //info->flags = clap_param_info_flags
  //info->module;
  //info->name;
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