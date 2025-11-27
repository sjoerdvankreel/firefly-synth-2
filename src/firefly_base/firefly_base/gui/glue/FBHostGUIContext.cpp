#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
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

FBHostGUIContext::
FBHostGUIContext(std::unique_ptr<FBStaticTopo>&& topo):
_topo(std::make_unique<FBRuntimeTopo>(std::move(topo))),
_guiState(std::make_unique<FBGUIStateContainer>(*_topo)),
_exchangeFromDSPState(std::make_unique<FBExchangeStateContainer>(*_topo)),
_undoState(this),
_patchState(*_topo.get()),
_sessionState(*_topo.get())
{
}

void 
FBHostGUIContext::RevertToPatchState()
{
  _patchState.CopyTo(this);
}

void
FBHostGUIContext::MarkAsSessionState()
{
  _sessionState.CopyFrom(this);
}

void
FBHostGUIContext::RevertToSessionState()
{
  _sessionState.CopyTo(this);
}

std::string const& 
FBHostGUIContext::PatchName() const 
{ 
  return _guiState->PatchName(); 
}

void
FBHostGUIContext::OnPatchLoaded()
{
  for (int i = 0; i < _listeners.size(); i++)
    _listeners[i]->OnPatchLoaded();
}

void
FBHostGUIContext::OnPatchNameChanged()
{
  for (int i = 0; i < _listeners.size(); i++)
    _listeners[i]->OnPatchNameChanged(PatchName());
}

void 
FBHostGUIContext::SetPatchName(std::string const& name)
{
  _guiState->SetPatchName(name);
  OnPatchNameChanged();
}

void 
FBHostGUIContext::AddListener(IFBHostGUIContextListener* listener)
{
  auto iter = std::find(_listeners.begin(), _listeners.end(), listener);
  if (iter == _listeners.end())
    _listeners.push_back(listener);
}

void 
FBHostGUIContext::RemoveListener(IFBHostGUIContextListener* listener)
{
  auto iter = std::find(_listeners.begin(), _listeners.end(), listener);
  if (iter != _listeners.end())
    _listeners.erase(iter);
}

void
FBHostGUIContext::MarkAsPatchState(std::string const& name)
{
  _patchState.CopyFrom(this);
  _isPatchLoaded = true;
  OnPatchLoaded();
  SetPatchName(name);
}

double 
FBHostGUIContext::GetGUIParamNormalized(int index) const 
{ 
  return *_guiState->Params()[index];
}

void 
FBHostGUIContext::SetGUIParamNormalized(int index, double normalized) 
{ 
  *_guiState->Params()[index] = normalized;
}

double 
FBHostGUIContext::GetUserScaleMin() const
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return 1.0;
  return _topo->static_->modules[m].guiParams[p].Linear().min;
}

double 
FBHostGUIContext::GetUserScaleMax() const
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return 1.0;
  return _topo->static_->modules[m].guiParams[p].Linear().max;
}

double 
FBHostGUIContext::GetUserScalePlain() const
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return 1.0;
  double norm = *_guiState->Params()[_topo->gui.ParamAtTopo({ { m, 0 }, { p, 0 } })->runtimeParamIndex];
  return _topo->static_->modules[m].guiParams[p].NonRealTime().NormalizedToPlain(norm);
}

void 
FBHostGUIContext::SetUserScalePlain(double scale)
{
  int m = _topo->static_->guiUserScaleModule;
  int p = _topo->static_->guiUserScaleParam;
  if (m == -1)
    return;
  double norm = _topo->static_->modules[m].guiParams[p].NonRealTime().PlainToNormalized(scale);
  *_guiState->Params()[_topo->gui.ParamAtTopo({ { m, 0 }, { p, 0 } })->runtimeParamIndex] = norm;
}

void
FBHostGUIContext::BeginAudioParamChange(int index)
{
  DoBeginAudioParamChange(index);
}

void
FBHostGUIContext::EndAudioParamChange(int index)
{
  DoEndAudioParamChange(index);
}

void
FBHostGUIContext::PerformAudioParamEdit(int index, double normalized)
{
  DoPerformAudioParamEdit(index, normalized);
}

void
FBHostGUIContext::PerformImmediateAudioParamEdit(FBParamTopoIndices const& indices, double normalized)
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  PerformImmediateAudioParamEdit(param->runtimeParamIndex, normalized);
}

void
FBHostGUIContext::PerformImmediateAudioParamEdit(int index, double normalized)
{
  BeginAudioParamChange(index);
  PerformAudioParamEdit(index, normalized);
  EndAudioParamChange(index);
}

std::string
FBHostGUIContext::GetAudioParamText(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->NormalizedToTextWithUnit(false, normalized);
}

double
FBHostGUIContext::GetAudioParamNormalized(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  return GetAudioParamNormalized(param->runtimeParamIndex);
}

int
FBHostGUIContext::GetAudioParamDiscrete(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Discrete().NormalizedToPlainFast(static_cast<float>(normalized));
}

double
FBHostGUIContext::GetAudioParamLinear(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedToPlainFast(static_cast<float>(normalized));
}

double
FBHostGUIContext::GetAudioParamIdentity(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Identity().NormalizedToPlainFast(static_cast<float>(normalized));
}

bool 
FBHostGUIContext::GetAudioParamBool(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
}

bool
FBHostGUIContext::GetGUIParamBool(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
}

int 
FBHostGUIContext::GetGUIParamDiscrete(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Discrete().NormalizedToPlainFast(static_cast<float>(normalized));
}

void
FBHostGUIContext::SetGUIParamList(FBParamTopoIndices const& indices, int val)
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = param->static_.ListNonRealTime().PlainToNormalized(val);
  SetGUIParamNormalized(param->runtimeParamIndex, normalized);
}

void 
FBHostGUIContext::SetGUIParamBool(FBParamTopoIndices const& indices, bool val)
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = param->static_.BooleanNonRealTime().PlainToNormalized(val ? 1.0 : 0.0);
  SetGUIParamNormalized(param->runtimeParamIndex, normalized);
}

void 
FBHostGUIContext::SetGUIParamDiscrete(FBParamTopoIndices const& indices, int val)
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = param->static_.DiscreteNonRealTime().PlainToNormalized(val);
  SetGUIParamNormalized(param->runtimeParamIndex, normalized);
}

void 
FBHostGUIContext::DefaultAudioParam(FBParamTopoIndices const& indices)
{
  auto runtimeParam = Topo()->audio.ParamAtTopo(indices);
  double normalized = runtimeParam->DefaultNormalizedByText();
  PerformImmediateAudioParamEdit(runtimeParam->runtimeParamIndex, normalized);
}

void 
FBHostGUIContext::CopyAudioParam(FBParamTopoIndices const& from, FBParamTopoIndices const& to)
{
  auto fromRuntimeParam = Topo()->audio.ParamAtTopo(from);
  auto toRuntimeParam = Topo()->audio.ParamAtTopo(to);
  auto fromValue = GetAudioParamNormalized(fromRuntimeParam->runtimeParamIndex);
  PerformImmediateAudioParamEdit(toRuntimeParam->runtimeParamIndex, fromValue);
}

void 
FBHostGUIContext::ClearModuleAudioParams(FBTopoIndices const& moduleIndices)
{  
  auto const& staticModule = Topo()->static_->modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
    for (int s = 0; s < staticModule.params[p].slotCount; s++)
      DefaultAudioParam({ moduleIndices, { p, s } });
}

void 
FBHostGUIContext::CopyModuleAudioParams(FBTopoIndices const& moduleIndices, int toSlot)
{  
  auto const& staticModule = Topo()->static_->modules[moduleIndices.index];
  for (int p = 0; p < staticModule.params.size(); p++)
    for (int s = 0; s < staticModule.params[p].slotCount; s++)
      CopyAudioParam({ moduleIndices, { p, s } }, { { moduleIndices.index, toSlot }, { p, s } });
}

std::shared_ptr<FBPresetFolder>
FBHostGUIContext::LoadPresetList(std::filesystem::path const& p) const
{
  auto result = std::make_shared<FBPresetFolder>();
  result->name = p.string();
  for (auto i = p.begin(); i != p.end(); i++)
  {
    if (std::filesystem::is_regular_file(*i))
    {
      FBPresetFile file = {};
      file.path = i->string();
      file.name = i->stem().string();
      result->files.push_back(file);
    }
    if (std::filesystem::is_directory(*i))
      result->folders.push_back(LoadPresetList(*i));
  }
  return result;
}

std::shared_ptr<FBPresetFolder>
FBHostGUIContext::LoadPresetList() const
{
  std::string subPath = Topo()->static_->meta.isFx ? "fx" : "instrument";
  std::filesystem::path presetRoot(FBGetResourcesFolderPath() / "presets" / subPath);
  if (std::filesystem::exists(presetRoot))
    return LoadPresetList(presetRoot);
  return {};
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