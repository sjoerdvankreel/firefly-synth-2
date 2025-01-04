#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>

bool 
FBCLAPPlugin::implementsParams() const noexcept 
{ 
  return true; 
}

uint32_t 
FBCLAPPlugin::paramsCount() const noexcept
{
  return static_cast<uint32_t>(_topo->params.size());
}

bool 
FBCLAPPlugin::paramsValue(
  clap_id paramId, double* value) noexcept
{
  int32_t index = getParamIndexForParamId(paramId);
  if (index == -1)
    return false;
  auto const& state = _state;
  auto const& static_ = _topo->params[index].static_;
  *value = FBNormalizedToCLAP(static_, state.Params()[index].Value());
  return true;
}

bool
FBCLAPPlugin::paramsTextToValue(
  clap_id paramId, const char* display, double* value) noexcept
{
  int32_t index = getParamIndexForParamId(paramId);
  if (index == -1)
    return false;
  auto const& static_ = _topo->params[index].static_;
  auto normalized = static_.TextToNormalized(display, false);
  if (!normalized.has_value())
    return false;
  *value = FBNormalizedToCLAP(static_, normalized.value());
  return true;
}

bool
FBCLAPPlugin::paramsValueToText(
  clap_id paramId, double value, char* display, uint32_t size) noexcept
{
  int32_t index = getParamIndexForParamId(paramId);
  if (index == -1)
    return false;
  float normalized = FBCLAPToNormalized(_topo->params[index].static_, value);
  std::string text = _topo->params[index].static_.NormalizedToText(false, normalized);
  std::fill(display, display + size, 0);
  strncpy(display, text.c_str(), std::min(size - 1, static_cast<uint32_t>(text.size())));
  return true;
}

void
FBCLAPPlugin::paramsFlush(
  const clap_input_events* in, const clap_output_events* out) noexcept
{
#if 0 // TODO
  // also todo stuff with claptonorm
  // TODO when is this even called anyway?
  // TODO handle case with gui / main thread
  for (uint32_t i = 0; i < in->size(in); i++)
  {
    clap_event_header_t const* header = in->get(in, i);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID)
      continue;
    if (header->type != CLAP_EVENT_PARAM_VALUE)
      continue;
    auto event = reinterpret_cast<clap_event_param_value const*>(header);
    int index = getParamIndexForParamId(event->param_id);
    if (index == -1)
      continue;
    if (_procStatePtrs.isAcc[index])
      _procStatePtrs.single.acc[index]->current = static_cast<float>(event->value);
    else
      *_procStatePtrs.single.block[index] = static_cast<float>(event->value);
  }
#endif  
}

bool 
FBCLAPPlugin::paramsInfo(
  uint32_t paramIndex, clap_param_info* info) const noexcept
{
  auto const& runtimeParam = _topo->params[paramIndex];
  auto const& staticParam = runtimeParam.static_;
  auto const& runtimeModule = _topo->modules[runtimeParam.runtimeModuleIndex];
  auto const& staticModule = _topo->static_.modules[runtimeParam.staticModuleIndex];
  if (paramIndex >= _topo->params.size())
    return false;

  info->min_value = 0.0;
  info->cookie = nullptr;
  info->id = runtimeParam.tag;
  info->default_value = FBNormalizedToCLAP(staticParam, staticParam.DefaultNormalizedByText());

  std::fill(info->name, info->name + sizeof(info->name), 0);
  std::fill(info->module, info->module + sizeof(info->module), 0);
  strncpy(info->module, runtimeModule.name.c_str(),
    std::min(sizeof(info->module) - 1, runtimeModule.name.size()));
  strncpy(info->name, runtimeParam.longName.c_str(),
    std::min(sizeof(info->name) - 1, runtimeParam.longName.size()));

  // TODO CLAP_PARAM_IS_HIDDEN
  // TODO CLAP_PARAM_IS_READONLY
  info->flags = CLAP_PARAM_IS_AUTOMATABLE; // TODO need this also for vst3? bitwig requires to show host controls
  if (staticParam.valueCount != 0)
  {
    info->flags |= CLAP_PARAM_IS_STEPPED;
    info->max_value = staticParam.valueCount - 1.0f;
    if (staticParam.type == FBParamType::List)
      info->flags |= CLAP_PARAM_IS_ENUM;
  }
  else
  {
    info->max_value = 1.0;
    info->flags |= CLAP_PARAM_IS_MODULATABLE;
    if (staticModule.voice)
    {
      info->flags |= CLAP_PARAM_IS_MODULATABLE_PER_KEY;
      info->flags |= CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL;
      info->flags |= CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID;
    }
  }
  return true;
}