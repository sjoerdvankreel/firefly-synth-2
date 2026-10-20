#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>

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
  else if (kind == CLAP_CONTEXT_MENU_ITEM_BEGIN_SUBMENU)
  {
    auto begin = static_cast<clap_context_menu_submenu const*>(data);
    item.name = begin->label;
    item.enabled = begin->is_enabled;
  }
  static_cast<std::vector<FBHostContextMenuItem>*>(builder->ctx)->push_back(item);
  return true;
}

void 
FBCLAPPlugin::NotifyHostOfParamNameChanges()
{
  _host.paramsRescan(CLAP_PARAM_RESCAN_INFO);
}

void
FBCLAPPlugin::ClearAudioParamNameOverrides()
{
  _audioParamNameOverridesByIndex.clear();
}

void
FBCLAPPlugin::ClearAudioParamNameOverride(int index)
{
  _audioParamNameOverridesByIndex.erase(index);
}

void
FBCLAPPlugin::SetAudioParamNameOverride(int index, std::string const& name)
{
  _audioParamNameOverridesByIndex[index] = name;
}

bool 
FBCLAPPlugin::GetAudioParamNameOverride(int index, std::string& name) const
{
  auto iter = _audioParamNameOverridesByIndex.find(index);
  if (iter == _audioParamNameOverridesByIndex.end())
    return false;
  name = iter->second;
  return true;
}

void
FBCLAPPlugin::ClearDSPAudioClipToDSP(std::string const& id)
{
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  _writingAudioClipsToDSPIsDirty.store(true);
  FBCLAPAudioClipToDSPCommand command = {};
  command.id = id;
  command.Type = FBCLAPAudioClipToDSPCommand::Clear;
  _audioClipToDSPCommands.enqueue(command);
}

void
FBCLAPPlugin::PushDSPAudioClipToDSP(std::string const& id, FBAudioClip const& clip)
{
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  _writingAudioClipsToDSPIsDirty.store(true);
  FBCLAPAudioClipToDSPCommand command = {};
  command.id = id;
  command.Type = FBCLAPAudioClipToDSPCommand::Single;
  command.clip = FBAudioClip(clip);
  _audioClipToDSPCommands.enqueue(command);
}

void 
FBCLAPPlugin::PushDSPAudioClipsToDSP(std::map<std::string, FBAudioClip> const& dspClips)
{
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  _writingAudioClipsToDSPIsDirty.store(true);
  FBCLAPAudioClipToDSPCommand command = {};
  command.Type = FBCLAPAudioClipToDSPCommand::All;
  command.allClips = dspClips;
  _audioClipToDSPCommands.enqueue(command);
}

std::map<std::string, FBAudioClip>
FBCLAPPlugin::PullDSPAudioClipsToGUI() const
{
  // OK so for CLAP this is "easy":
  // we're not in the whole vst3 separation stuff so:
  // 1. on any push we set the dsp-is-dirty-flag
  // 2. processor eventually picks up
  // 3. we lock on not-push-now mutex everywhere from gui->dsp
  // 4. wait for dirty flag to become unset WHILE still holding the lock
  // 5. any writers have finished now (they are all gui side)
  // 6. we are ready to copy from in-memory
  // 7. we should be lock-free & safe on dsp. I think. I hope.
  std::lock_guard lock(_blockWritingAudioClipsToDSP);

  // Sue me. I should probably bring in a condition variable here.
  while (_writingAudioClipsToDSPIsDirty.load());
  return GetDSPAudioClips();
}

double
FBCLAPPlugin::GetAudioParamNormalized(int index) const
{
  return *_editState->Params()[index];
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