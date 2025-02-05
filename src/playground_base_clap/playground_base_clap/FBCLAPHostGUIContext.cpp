#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>

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

float
FBCLAPPlugin::GetParamNormalized(int index) const
{
  return *_editState->Params()[index];
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
  *_editState->Params()[index] = normalized;
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::PerformEdit, index, normalized);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

std::vector<FBHostContextMenuItem>
FBCLAPPlugin::MakeParamContextMenu(int index)
{
  std::vector<FBHostContextMenuItem> items;
  if (!MakeParamContextMenu(index, items))
    return {};
  return items;
}

void
FBCLAPPlugin::ParamContextMenuClicked(int paramIndex, int juceTag)
{
  std::vector<FBHostContextMenuItem> items;
  auto target = MakeParamContextMenu(paramIndex, items);
  if (target)
    _host.contextMenuPerform(target.get(), items[juceTag - 1].hostTag);
}

std::unique_ptr<clap_context_menu_target>
FBCLAPPlugin::MakeParamContextMenu(int index, std::vector<FBHostContextMenuItem>& items)
{
  if (!_host.canUseContextMenu())
    return {};

  clap_context_menu_builder builder;
  builder.ctx = &items;
  builder.add_item = ContextMenuBuilderAddItem;
  builder.supports = [](auto, auto) { return true; };

  auto result = std::make_unique<clap_context_menu_target>();
  result->id = _topo->params[index].tag;
  result->kind = CLAP_CONTEXT_MENU_TARGET_KIND_PARAM;
  // bitwig returns false
  _host.contextMenuPopulate(result.get(), &builder);
  return result;
}