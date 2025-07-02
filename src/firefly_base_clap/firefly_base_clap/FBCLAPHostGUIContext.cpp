#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

#include <clap/helpers/plugin.hxx>

using namespace juce;

static bool CLAP_ABI
ContextMenuBuilderAddItem(
  clap_context_menu_builder const* builder,
  clap_context_menu_item_kind_t kind,
  void const* data)
{
  FBHostContextMenuItem item = {};
  item.separator = kind == CLAP_CONTEXT_MENU_ITEM_SEPARATOR;
  item.subMenuEnd = kind == CLAP_CONTEXT_MENU_ITEM_END_SUBMENU;
  item.subMenuStart = kind == CLAP_CONTEXT_MENU_ITEM_BEGIN_SUBMENU;
  if (kind == CLAP_CONTEXT_MENU_ITEM_TITLE)
  {
    auto title = static_cast<clap_context_menu_item_title const*>(data);
    item.name = title->title;
    item.enabled = title->is_enabled;
  }
  else if (kind == CLAP_CONTEXT_MENU_ITEM_ENTRY)
  {
    auto entry = static_cast<clap_context_menu_entry const*>(data);
    item.name = entry->label;
    item.hostTag = entry->action_id;
    item.enabled = entry->is_enabled;
  }
  else if (kind == CLAP_CONTEXT_MENU_ITEM_CHECK_ENTRY)
  {
    auto check = static_cast<clap_context_menu_check_entry const*>(data);
    item.name = check->label;
    item.hostTag = check->action_id;
    item.enabled = check->is_enabled;
    item.checked = check->is_checked;
  }
  static_cast<std::vector<FBHostContextMenuItem>*>(builder->ctx)->push_back(item);
  return true;
}

double
FBCLAPPlugin::GetAudioParamNormalized(int index) const
{
  return *_editState->Params()[index];
}

double
FBCLAPPlugin::GetGUIParamNormalized(int index) const
{
  return *_guiState->Params()[index];
}

void
FBCLAPPlugin::SetGUIParamNormalized(int index, double normalized)
{
  *_guiState->Params()[index] = normalized;
}

void
FBCLAPPlugin::DoEndAudioParamChange(int index)
{
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::EndChange, index, 0.0f);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

void
FBCLAPPlugin::DoBeginAudioParamChange(int index)
{
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::BeginChange, index, 0.0f);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

void
FBCLAPPlugin::DoPerformAudioParamEdit(int index, double normalized)
{
  *_editState->Params()[index] = normalized;
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::PerformEdit, index, normalized);
  _mainToAudioEvents.enqueue(event);
  if (_gui)
    _gui->SetAudioParamNormalizedFromHost(index, normalized);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

std::vector<FBHostContextMenuItem>
FBCLAPPlugin::MakeAudioParamContextMenu(int index)
{
  std::vector<FBHostContextMenuItem> items;
  if (!MakeAudioParamContextMenu(index, items))
    return {};
  return items;
}

void
FBCLAPPlugin::AudioParamContextMenuClicked(int paramIndex, int juceTag)
{
  FBWithLogException([this, paramIndex, juceTag]()
  {
    std::vector<FBHostContextMenuItem> items;
    auto target = MakeAudioParamContextMenu(paramIndex, items);
    if (target)
      _host.contextMenuPerform(target.get(), items[juceTag - 1].hostTag);
  });
}

std::unique_ptr<clap_context_menu_target>
FBCLAPPlugin::MakeAudioParamContextMenu(int index, std::vector<FBHostContextMenuItem>& items)
{
  return FBWithLogException([this, index, &items]()
  {
    if (!_host.canUseContextMenu())
      return std::unique_ptr<clap_context_menu_target>();

    clap_context_menu_builder builder;
    builder.ctx = &items;
    builder.add_item = ContextMenuBuilderAddItem;
    builder.supports = [](auto, auto) { return true; };

    auto result = std::make_unique<clap_context_menu_target>();
    result->id = _topo->audio.params[index].tag;
    result->kind = CLAP_CONTEXT_MENU_TARGET_KIND_PARAM;
    // bitwig returns false
    _host.contextMenuPopulate(result.get(), &builder);
    return result;
  });
}