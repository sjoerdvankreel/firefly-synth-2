#include <firefly_base_vst3/FBVST3Plugin.hpp>
#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>

void 
FBVST3Plugin::OnReceivedExchangeFromDSP()
{
  if (_guiEditor)
    _guiEditor->UpdateExchangeState();
}

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
FBVST3Plugin::NotifyHostOfParamNameChangesOnly()
{
  if (componentHandler)
    componentHandler->restartComponent(Vst::kParamTitlesChanged);
}

void 
FBVST3Plugin::NotifyHostOfBulkUpdateAndParamNameChanges()
{
  if (componentHandler)
    componentHandler->restartComponent(Vst::kParamValuesChanged | Vst::kParamTitlesChanged);
}

void 
FBVST3Plugin::BulkUpdateAudioParamsToGUI(FBScalarStateContainer const& audioParams, bool patchOnly)
{
  for (int p = 0; p < audioParams.Params().size(); p++)
    if (!patchOnly || Topo()->audio.params[p].static_.storeInPatch)
      setParamNormalized(Topo()->audio.params[p].tag, *audioParams.Params()[p]);
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