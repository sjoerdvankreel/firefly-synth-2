#include <firefly_base/base/shared/FBHostContext.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <stack>
#include <memory>
#include <filesystem>

using namespace juce;

struct MenuBuilder
{
  bool checked = false;
  bool enabled = false;
  std::string name = {};
  std::shared_ptr<PopupMenu> menu = {};
};

FBHostContext::
~FBHostContext()
{
}

FBHostContext::
FBHostContext(std::unique_ptr<FBStaticTopo>&& topo):
_topo(std::make_unique<FBRuntimeTopo>(std::move(topo))),
_undoState(this),
_patchState(*_topo.get()),
_sessionState(*_topo.get()),
_guiState(std::make_unique<FBGUIStateContainer>(*_topo)),
_procState(std::make_unique<FBProcStateContainer>(*topo, &_dspAudioClips)),
_exchangeToGUIState(std::make_unique<FBExchangeStateContainer>(*topo)),
_exchangeFromDSPState(std::make_unique<FBExchangeStateContainer>(*_topo))
{
}

std::map<int, std::string> 
FBHostContext::GetAudioParamNameOverrides() const
{
  std::string name;
  std::map<int, std::string> result;
  for (int i = 0; i < _topo->audio.params.size(); i++)
    if (GetAudioParamNameOverride(i, name))
      result[i] = name;
  return result;
}

FBAudioClip const* 
FBHostContext::FindGUIAudioClip(std::string const& id) const
{
  auto iter = _guiAudioClips.find(id);
  if (iter == _guiAudioClips.end())
    return nullptr;
  return &iter->second;
}

void
FBHostContext::SetGUIAudioClips(std::map<std::string, FBAudioClip> const& clips)
{
  _undoState.Clear();
  _guiAudioClips = clips;
  OnGUIAudioClipsChanged();
}

void 
FBHostContext::ClearGUIAndDSPAudioClips()
{
  _undoState.Clear();
  for (auto const& kv : _guiAudioClips)
    ClearDSPAudioClipToDSP(kv.first);
  _guiAudioClips.clear();
  OnGUIAudioClipsChanged();
}

void 
FBHostContext::RemoveGUIAndDSPAudioClip(std::string const& id)
{ 
  _undoState.Clear();
  ClearDSPAudioClipToDSP(id);
  _guiAudioClips.erase(id);
  OnGUIAudioClipsChanged();
}

void
FBHostContext::SetGUIAndDSPAudioClip(
  std::string const& id, FBAudioClip const& guiClip, FBAudioClip const& dspClip)
{
  _undoState.Clear();
  _guiAudioClips[id] = FBAudioClip(guiClip);
  PushDSPAudioClipToDSP(id, dspClip);
  OnGUIAudioClipsChanged();
}

void 
FBHostContext::SetAudioParamNameOverrides(std::map<int, std::string> const& overrides)
{
  ClearAudioParamNameOverrides();
  for (auto kv : overrides)
    SetAudioParamNameOverride(kv.first, kv.second);
}

void
FBHostContext::MarkPatchAsSessionState()
{
  _sessionState.CopyFrom(this, true);
}

void
FBHostContext::MarkPatchAsPatchState(std::string const& patchName)
{
  _patchState.CopyFrom(this, true);
  _isPatchLoaded = true;
  OnPatchLoaded();
  SetPatchName(patchName);
}

std::string const& 
FBHostContext::PatchName() const
{ 
  return _guiState->PatchName(); 
}

std::string const&
FBHostContext::ThemeName() const
{
  return _guiState->ThemeName();
}

std::string const&
FBHostContext::InstanceName() const
{
  return _guiState->InstanceName();
}

void
FBHostContext::OnPatchLoaded()
{
  for (int i = 0; i < _guiListeners.size(); i++)
    _guiListeners[i]->OnPatchLoaded();
}

void
FBHostContext::OnPatchNameChanged()
{
  for (int i = 0; i < _guiListeners.size(); i++)
    _guiListeners[i]->OnPatchNameChanged(PatchName());
}

void
FBHostContext::OnGUIAudioClipsChanged()
{
  for (int i = 0; i < _guiListeners.size(); i++)
    _guiListeners[i]->OnGUIAudioClipsChanged();
}

void
FBHostContext::OnInstanceNameChanged()
{
  for (int i = 0; i < _guiListeners.size(); i++)
    _guiListeners[i]->OnInstanceNameChanged(InstanceName());
}

void 
FBHostContext::SetPatchName(std::string const& name)
{
  _guiState->SetPatchName(name);
  OnPatchNameChanged();
}

void
FBHostContext::SetInstanceName(std::string const& name)
{
  _guiState->SetInstanceName(name);
  OnInstanceNameChanged();
}

void
FBHostContext::SetThemeName(std::string const& name)
{
  _guiState->SetThemeName(name);
}

void 
FBHostContext::AddGUIListener(IFBHostContextGUIListener* listener)
{
  auto iter = std::find(_guiListeners.begin(), _guiListeners.end(), listener);
  if (iter == _guiListeners.end())
    _guiListeners.push_back(listener);
}

void 
FBHostContext::RemoveGUIListener(IFBHostContextGUIListener* listener)
{
  auto iter = std::find(_guiListeners.begin(), _guiListeners.end(), listener);
  if (iter != _guiListeners.end())
    _guiListeners.erase(iter);
}

double 
FBHostContext::GetGUIParamNormalized(int index) const
{ 
  return *_guiState->Params()[index];
}

void 
FBHostContext::SetGUIParamNormalized(int index, double normalized)
{ 
  *_guiState->Params()[index] = normalized;
}

double 
FBHostContext::GetUserScaleMin() const
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return 1.0;
  return _topo->static_->modules[m].guiParams[p].Linear().min;
}

double 
FBHostContext::GetUserScaleMax() const
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return 1.0;
  return _topo->static_->modules[m].guiParams[p].Linear().max;
}

double 
FBHostContext::GetUserScalePlain() const
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return 1.0;
  double norm = *_guiState->Params()[_topo->gui.ParamAtTopo({ { m, 0 }, { p, 0 } })->runtimeParamIndex];
  return _topo->static_->modules[m].guiParams[p].NonRealTime().NormalizedToPlain(norm);
}

void 
FBHostContext::SetUserScalePlain(double scale)
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return;
  double norm = _topo->static_->modules[m].guiParams[p].NonRealTime().PlainToNormalized(scale);
  *_guiState->Params()[_topo->gui.ParamAtTopo({ { m, 0 }, { p, 0 } })->runtimeParamIndex] = norm;
}

void
FBHostContext::BeginAudioParamChange(int index)
{
  DoBeginAudioParamChange(index);
}

void
FBHostContext::EndAudioParamChange(int index)
{
  DoEndAudioParamChange(index);
}

void
FBHostContext::PerformAudioParamEdit(int index, double normalized)
{
  DoPerformAudioParamEdit(index, normalized);
}

void
FBHostContext::PerformImmediateAudioParamEdit(FBParamTopoIndices const& indices, double normalized)
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  PerformImmediateAudioParamEdit(param->runtimeParamIndex, normalized);
}

void
FBHostContext::PerformImmediateAudioParamEdit(int index, double normalized)
{
  BeginAudioParamChange(index);
  PerformAudioParamEdit(index, normalized);
  EndAudioParamChange(index);
}

std::string
FBHostContext::GetAudioParamText(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->NormalizedToTextWithUnit(false, normalized);
}

double
FBHostContext::GetAudioParamNormalized(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  return GetAudioParamNormalized(param->runtimeParamIndex);
}

int
FBHostContext::GetAudioParamDiscrete(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Discrete().NormalizedToPlainFast(static_cast<float>(normalized));
}

void
FBHostContext::SetAudioParamDiscrete(FBParamTopoIndices const& indices, int value)
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = param->static_.DiscreteNonRealTime().PlainToNormalized(value);
  PerformImmediateAudioParamEdit(indices, normalized);
}

double
FBHostContext::GetAudioParamLinear(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedToPlainFast(static_cast<float>(normalized));
}

double
FBHostContext::GetAudioParamIdentity(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Identity().NormalizedToPlainFast(static_cast<float>(normalized));
}

bool 
FBHostContext::GetAudioParamBool(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
}

void
FBHostContext::SetAudioParamBool(FBParamTopoIndices const& indices, bool val)
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = param->static_.BooleanNonRealTime().PlainToNormalized(val);
  PerformImmediateAudioParamEdit(indices, normalized);
}

bool
FBHostContext::GetGUIParamBool(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
}

int 
FBHostContext::GetGUIParamDiscrete(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Discrete().NormalizedToPlainFast(static_cast<float>(normalized));
}

void
FBHostContext::SetGUIParamList(FBParamTopoIndices const& indices, int val)
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = param->static_.ListNonRealTime().PlainToNormalized(val);
  SetGUIParamNormalized(param->runtimeParamIndex, normalized);
}

void 
FBHostContext::SetGUIParamBool(FBParamTopoIndices const& indices, bool val)
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = param->static_.BooleanNonRealTime().PlainToNormalized(val ? 1.0 : 0.0);
  SetGUIParamNormalized(param->runtimeParamIndex, normalized);
}

void 
FBHostContext::SetGUIParamDiscrete(FBParamTopoIndices const& indices, int val)
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = param->static_.DiscreteNonRealTime().PlainToNormalized(val);
  SetGUIParamNormalized(param->runtimeParamIndex, normalized);
}

void 
FBHostContext::DefaultAudioParam(FBParamTopoIndices const& indices)
{
  auto runtimeParam = Topo()->audio.ParamAtTopo(indices);
  double normalized = runtimeParam->DefaultNormalizedByText();
  PerformImmediateAudioParamEdit(runtimeParam->runtimeParamIndex, normalized);
}

void 
FBHostContext::CopyAudioParam(FBParamTopoIndices const& from, FBParamTopoIndices const& to)
{
  auto fromRuntimeParam = Topo()->audio.ParamAtTopo(from);
  auto toRuntimeParam = Topo()->audio.ParamAtTopo(to);
  auto fromValue = GetAudioParamNormalized(fromRuntimeParam->runtimeParamIndex);
  PerformImmediateAudioParamEdit(toRuntimeParam->runtimeParamIndex, fromValue);
}

void 
FBHostContext::ClearModuleAudioParams(FBTopoIndices const& moduleIndices)
{  
  auto const& staticModule = Topo()->static_->modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
    for (int s = 0; s < staticModule.params[p].slotCount; s++)
      DefaultAudioParam({ moduleIndices, { p, s } });
}

void 
FBHostContext::CopyModuleAudioParams(FBTopoIndices const& moduleIndices, int toSlot)
{  
  auto const& staticModule = Topo()->static_->modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
    for (int s = 0; s < staticModule.params[p].slotCount; s++)
      CopyAudioParam({ moduleIndices, { p, s } }, { { moduleIndices.index, toSlot }, { p, s } });
}

void 
FBHostContext::ShowOnlineManual() const
{
  juce::URL(OnlineManualLocation()).launchInDefaultBrowser();
}

void
FBHostContext::ShowOnlineManualForGUIParam(int index) const
{
  int rtModuleIndex = Topo()->gui.params[index].runtimeModuleIndex;
  ShowOnlineManualForModule(rtModuleIndex);
}

void
FBHostContext::ShowOnlineManualForAudioParam(int index) const
{
  int rtModuleIndex = Topo()->audio.params[index].runtimeModuleIndex;
  ShowOnlineManualForModule(rtModuleIndex);
}

void
FBHostContext::ShowOnlineManualForModule(int index) const
{
  int staticModuleIndex = Topo()->modules[index].topoIndices.index;
  auto const& moduleId = Topo()->static_->modules[staticModuleIndex].id;
  auto cleanModuleId = FBCleanTopoId(moduleId);
  juce::URL(OnlineManualLocation()).withAnchor(cleanModuleId).launchInDefaultBrowser();
}

void
FBAddHostContextMenu(
  std::shared_ptr<juce::PopupMenu> menu, 
  int offset, 
  std::vector<FBHostContextMenuItem> const& items)
{
  std::stack<MenuBuilder> builders = {};
  builders.emplace();
  builders.top().menu = menu;
  for (int i = 0; i < items.size(); i++)
    if (items[i].subMenuStart)
    {
      builders.emplace();
      builders.top().name = items[i].name;
      builders.top().checked = items[i].checked;
      builders.top().enabled = items[i].enabled;
      builders.top().menu = std::make_shared<PopupMenu>();
    } else if (items[i].subMenuEnd)
    {
      auto builder = std::move(builders.top());
      builders.pop();
      std::string name = builder.name.size() ? builder.name : items[i].name;
      builders.top().menu->addSubMenu(name, *builder.menu, builder.enabled, nullptr, builder.checked);
    } else if (items[i].separator)
      builders.top().menu->addSeparator();
    else
      builders.top().menu->addItem(i + 1 + offset, items[i].name, items[i].enabled, items[i].checked);
  FB_ASSERT(builders.size() == 1);
}