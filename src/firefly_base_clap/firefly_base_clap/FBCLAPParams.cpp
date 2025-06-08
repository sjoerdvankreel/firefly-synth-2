#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base_clap/FBCLAPUtility.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>
#include <firefly_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

bool 
FBCLAPPlugin::implementsParams() const noexcept 
{ 
  return true; 
}

uint32_t 
FBCLAPPlugin::paramsCount() const noexcept
{
  return static_cast<uint32_t>(_topo->audio.params.size());
}

bool 
FBCLAPPlugin::paramsValue(
  clap_id paramId, double* value) noexcept
{
  return FBWithLogException([this, paramId, value]()
  {
    int32_t index = getParamIndexForParamId(paramId);
    if (index == -1)
      return false;
    auto const& state = _editState;
    auto const& static_ = _topo->audio.params[index].static_;
    *value = FBNormalizedToCLAP(static_, *state->Params()[index]);
    return true;
  });
}

bool
FBCLAPPlugin::paramsTextToValue(
  clap_id paramId, const char* display, double* value) noexcept
{
  return FBWithLogException([this, paramId, display, value]()
  {
    int32_t index = getParamIndexForParamId(paramId);
    if (index == -1)
      return false;
    auto const& static_ = _topo->audio.params[index].static_;
    auto normalized = static_.NonRealTime().TextToNormalized(false, display);
    if (!normalized.has_value())
      return false;
    *value = FBNormalizedToCLAP(static_, normalized.value());
    return true;
  });
}

bool
FBCLAPPlugin::paramsValueToText(
  clap_id paramId, double value, char* display, uint32_t size) noexcept
{
  return FBWithLogException([this, paramId, value, display, size]()
  {
    int32_t index = getParamIndexForParamId(paramId);
    if (index == -1)
      return false;
    double norm = FBCLAPToNormalized(_topo->audio.params[index].static_, value);
    std::string text = _topo->audio.params[index].static_.NonRealTime().NormalizedToText(false, norm);
    std::fill(display, display + size, 0);
    strncpy(display, text.c_str(), std::min(size - 1, static_cast<uint32_t>(text.size())));
    return true;
  });
}

void
FBCLAPPlugin::paramsFlush(
  const clap_input_events* in, const clap_output_events* out) noexcept
{
  FBWithLogException([this, in, out]()
  {
    int inEventCount = in->size(in);
    for (int i = 0; i < inEventCount; i++)
    {
      auto header = in->get(in, i);
      if (header->space_id != CLAP_CORE_EVENT_SPACE_ID)
        continue;
      if (header->type != CLAP_EVENT_PARAM_VALUE)
        continue;
      auto event = reinterpret_cast<clap_event_param_value const*>(header);
      int index = getParamIndexForParamId(event->param_id);
      if (index == -1)
        continue;
      if (isProcessing())
      {
        _procState->InitProcessing(index, static_cast<float>(event->value));
        _audioToMainEvents.enqueue(FBMakeSyncToMainEvent(index, event->value));
      }
      else
      {
        *_editState->Params()[index] = event->value;
        _mainToAudioEvents.enqueue(FBMakeSyncToAudioEvent(FBCLAPSyncEventType::PerformEdit, index, event->value));
      }
    }

    ProcessMainToAudioEvents(out, false);
  });
}

bool 
FBCLAPPlugin::paramsInfo(
  uint32_t paramIndex, clap_param_info* info) const noexcept
{
  return FBWithLogException([this, paramIndex, info]()
  {
    auto const& runtimeParam = _topo->audio.params[paramIndex];
    auto const& staticParam = runtimeParam.static_;
    auto const& runtimeModule = _topo->modules[runtimeParam.runtimeModuleIndex];
    auto const& staticModule = _topo->static_.modules[runtimeParam.topoIndices.module.index];
    if (paramIndex >= _topo->audio.params.size())
      return false;

    info->min_value = 0.0;
    info->max_value = 1.0;
    info->cookie = nullptr;
    info->id = runtimeParam.tag;
    info->default_value = FBNormalizedToCLAP(staticParam, runtimeParam.DefaultNormalizedByText());

    std::fill(info->name, info->name + sizeof(info->name), 0);
    std::fill(info->module, info->module + sizeof(info->module), 0);
    strncpy(info->module, runtimeModule.name.c_str(),
      std::min(sizeof(info->module) - 1, runtimeModule.name.size()));
    strncpy(info->name, runtimeParam.longName.c_str(),
      std::min(sizeof(info->name) - 1, runtimeParam.longName.size()));

    info->flags = CLAP_PARAM_REQUIRES_PROCESS;
    auto timing = runtimeParam.static_.AutomationTiming();
    if (timing == FBAutomationTiming::Never)
    {
      info->flags |= CLAP_PARAM_IS_READONLY;
      if (runtimeParam.static_.NonRealTime().IsStepped())
      {
        info->flags |= CLAP_PARAM_IS_STEPPED;
        info->max_value = staticParam.NonRealTime().ValueCount() - 1.0;
        if (staticParam.NonRealTime().IsItems())
          info->flags |= CLAP_PARAM_IS_ENUM;
      }
    }
    else
    {
      info->flags |= CLAP_PARAM_IS_AUTOMATABLE;
      if (timing == FBAutomationTiming::PerSample)
      {
        info->flags |= CLAP_PARAM_IS_MODULATABLE;
        if (staticModule.voice)
        {
          info->flags |= CLAP_PARAM_IS_MODULATABLE_PER_KEY;
          info->flags |= CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL;
          info->flags |= CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID;
        }
      }
    }
    return true;
  });
}