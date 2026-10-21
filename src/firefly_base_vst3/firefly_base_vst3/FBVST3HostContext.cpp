#include <firefly_base_vst3/FBVST3Plugin.hpp>
#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>

void
FBVST3Plugin::DoEndAudioParamChange(int /*index*/)
{
  // see PerformAudioParamEdit
}

void
FBVST3Plugin::DoBeginAudioParamChange(int /*index*/)
{
  // see PerformAudioParamEdit
}

void
FBVST3Plugin::DoPerformAudioParamEdit(int index, double normalized)
{
  int tag = Topo()->audio.params[index].tag;
  setParamNormalized(tag, normalized);

  // this is not ideal but it prevents automation getting ignored
  // once an automated parameter is user-modified on reaper
  beginEdit(tag);
  performEdit(tag, normalized);
  endEdit(tag);
}

double
FBVST3Plugin::GetAudioParamNormalized(int index) const
{
  return parameters.getParameterByIndex(index)->getNormalized();
}

void
FBVST3Plugin::NotifyHostOfParamNameChanges()
{
  if (componentHandler)
    componentHandler->restartComponent(Vst::kParamTitlesChanged);
}

void
FBVST3Plugin::ClearAudioParamNameOverrides()
{
  for (int i = 0; i < parameters.getParameterCount(); i++)
    ClearAudioParamNameOverride(i);
}

void
FBVST3Plugin::ClearAudioParamNameOverride(int index)
{
  if (auto p = dynamic_cast<FBVST3Parameter*>(parameters.getParameterByIndex(index)))
    p->ClearNameOverride();
}

bool
FBVST3Plugin::GetAudioParamNameOverride(int index, std::string& name) const
{
  if (auto p = dynamic_cast<FBVST3Parameter*>(parameters.getParameterByIndex(index)))
    return p->GetNameOverride(name);
  return false;
}

void
FBVST3Plugin::SetAudioParamNameOverride(int index, std::string const& name)
{
  if (auto p = dynamic_cast<FBVST3Parameter*>(parameters.getParameterByIndex(index)))
    p->SetNameOverride(name);
}

std::vector<FBHostContextMenuItem>
FBVST3Plugin::MakeAudioParamContextMenu(int index)
{
  if (!_guiEditor)
    return {};
  return _guiEditor->MakeParamContextMenu(componentHandler, index);
}

void
FBVST3Plugin::AudioParamContextMenuClicked(int paramIndex, int juceTag)
{
  if (_guiEditor)
    _guiEditor->ParamContextMenuClicked(componentHandler, paramIndex, juceTag);
}

std::map<std::string, FBAudioClip>
FBVST3Plugin::PullDSPAudioClipsToGUI() const
{
  // Sue me. I should probably bring in a condition variable here.
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  while (_writingAudioClipsToDSPIsDirty.load());
  return GetDSPAudioClips();
}

void
FBVST3Plugin::ClearDSPAudioClipToDSP(std::string const& id)
{
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  _writingAudioClipsToDSPIsDirty.store(true);
  FBVST3AudioClipToDSPCommand command = {};
  command.id = id;
  command.Type = FBVST3AudioClipToDSPCommand::ClearSingle;
  _audioClipToDSPCommands.enqueue(command);
}

void
FBVST3Plugin::PushDSPAudioClipToDSP(std::string const& id, FBAudioClip const& clip)
{
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  _writingAudioClipsToDSPIsDirty.store(true);
  FBVST3AudioClipToDSPCommand command = {};
  command.id = id;
  command.Type = FBVST3AudioClipToDSPCommand::LoadSingle;
  command.clip = FBAudioClip(clip);
  _audioClipToDSPCommands.enqueue(command);
}

void
FBVST3Plugin::PushDSPAudioClipsToDSP(std::map<std::string, FBAudioClip> const& dspClips)
{
  std::lock_guard lock(_blockWritingAudioClipsToDSP);
  _writingAudioClipsToDSPIsDirty.store(true);
  FBVST3AudioClipToDSPCommand command = {};
  command.Type = FBVST3AudioClipToDSPCommand::LoadAll;
  command.allClips = dspClips;
  _audioClipToDSPCommands.enqueue(command);
}
