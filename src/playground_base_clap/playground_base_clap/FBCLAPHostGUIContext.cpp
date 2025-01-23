#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <clap/helpers/plugin.hxx>

using namespace juce;

float
FBCLAPPlugin::GetParamNormalized(int index) const
{
  return *_editState.Params()[index];
}

void
FBCLAPPlugin::EndParamChange(int index)
{
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::EndChange, index, 0.0f);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

void
FBCLAPPlugin::BeginParamChange(int index)
{
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::BeginChange, index, 0.0f);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

void
FBCLAPPlugin::PerformParamEdit(int index, float normalized)
{
  *_editState.Params()[index] = normalized;
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::PerformEdit, index, normalized);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

void 
FBCLAPPlugin::ParamContextMenuClicked(int paramIndex, int itemTag)
{
}

std::unique_ptr<PopupMenu> 
FBCLAPPlugin::MakeParamContextMenu(int index)
{
  if (!_host.canUseContextMenu())
    return {};

  return {};
}