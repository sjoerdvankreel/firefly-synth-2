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
  auto const& param = _topo->params[paramIndex];
  auto const& module = _topo->modules[param.runtimeModuleIndex];
  if (paramIndex >= _topo->params.size())
    return false;

  info->id = param.tag;
  info->max_value = 1.0;
  info->min_value = 0.0;
  info->cookie = nullptr;
  info->default_value = param.static_.defaultNormalized;
  std::fill(info->name, info->name + sizeof(info->name), 0);
  std::fill(info->module, info->module + sizeof(info->module), 0);
  strncpy(info->module, module.name.c_str(), module.name.size() - 1);
  strncpy(info->name, param.shortName.c_str(), param.shortName.size() - 1);

  info->flags = 0;

#if 0

    // Is this param stepped? (integer values only)
  // if so the double value is converted to integer using a cast (equivalent to trunc).
  CLAP_PARAM_IS_STEPPED = 1 << 0,

    // Useful for periodic parameters like a phase
    CLAP_PARAM_IS_PERIODIC = 1 << 1,

    // The parameter should not be shown to the user, because it is currently not used.
    // It is not necessary to process automation for this parameter.
    CLAP_PARAM_IS_HIDDEN = 1 << 2,

    // The parameter can't be changed by the host.
    CLAP_PARAM_IS_READONLY = 1 << 3,

    // This parameter is used to merge the plugin and host bypass button.
    // It implies that the parameter is stepped.
    // min: 0 -> bypass off
    // max: 1 -> bypass on
    CLAP_PARAM_IS_BYPASS = 1 << 4,

    // When set:
    // - automation can be recorded
    // - automation can be played back
    //
    // The host can send live user changes for this parameter regardless of this flag.
    //
    // If this parameter affects the internal processing structure of the plugin, ie: max delay, fft
    // size, ... and the plugins needs to re-allocate its working buffers, then it should call
    // host->request_restart(), and perform the change once the plugin is re-activated.
    CLAP_PARAM_IS_AUTOMATABLE = 1 << 5,

    // Does this parameter support per note automations?
    CLAP_PARAM_IS_AUTOMATABLE_PER_NOTE_ID = 1 << 6,

    // Does this parameter support per key automations?
    CLAP_PARAM_IS_AUTOMATABLE_PER_KEY = 1 << 7,

    // Does this parameter support per channel automations?
    CLAP_PARAM_IS_AUTOMATABLE_PER_CHANNEL = 1 << 8,

    // Does this parameter support per port automations?
    CLAP_PARAM_IS_AUTOMATABLE_PER_PORT = 1 << 9,

    // Does this parameter support the modulation signal?
    CLAP_PARAM_IS_MODULATABLE = 1 << 10,

    // Does this parameter support per note modulations?
    CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID = 1 << 11,

    // Does this parameter support per key modulations?
    CLAP_PARAM_IS_MODULATABLE_PER_KEY = 1 << 12,

    // Does this parameter support per channel modulations?
    CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL = 1 << 13,

    // Does this parameter support per port modulations?
    CLAP_PARAM_IS_MODULATABLE_PER_PORT = 1 << 14,

    // Any change to this parameter will affect the plugin output and requires to be done via
    // process() if the plugin is active.
    //
    // A simple example would be a DC Offset, changing it will change the output signal and must be
    // processed.
    CLAP_PARAM_REQUIRES_PROCESS = 1 << 15,

    // This parameter represents an enumerated value.
    // If you set this flag, then you must set CLAP_PARAM_IS_STEPPED too.
    // All values from min to max must not have a blank value_to_text().
    CLAP_PARAM_IS_ENUM = 1 << 16,

    info->name param.shortName
    //info->flags = clap_param_info_flags
    //info->module;
    //info->name;

#endif
    return true;
}

bool 
FBCLAPPlugin::paramsTextToValue(clap_id paramId, const char* display, double* value) noexcept
{
  return false;
}

void 
FBCLAPPlugin::paramsFlush(const clap_input_events* in, const clap_output_events* out) noexcept
{
}

bool 
FBCLAPPlugin::paramsValueToText(clap_id paramId, double value, char* display, uint32_t size) noexcept
{
  return false;
}