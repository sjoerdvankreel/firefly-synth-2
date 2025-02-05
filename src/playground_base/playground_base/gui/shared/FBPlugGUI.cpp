#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/shared/FBGUIConfig.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>

#include <cassert>

using namespace juce;

FBPlugGUI::
FBPlugGUI(FBHostGUIContext* hostContext) :
_hostContext(hostContext)
{
  _tooltipWindow = StoreComponent<TooltipWindow>();
  addAndMakeVisible(_tooltipWindow);
}

void
FBPlugGUI::SteppedParamNormalizedChanged(int index)
{
  for (auto target : _paramsDependents[index])
    target->DependenciesChanged();
}

void 
FBPlugGUI::InitAllDependencies()
{
  auto const& params = HostContext()->Topo()->params;
  for (int i = 0; i < params.size(); i++)
    if (FBParamTypeIsStepped(params[i].static_.type))
      SteppedParamNormalizedChanged(i);
}

void
FBPlugGUI::SetParamNormalizedFromHost(int index, float value)
{
  auto control = GetControlForParamIndex(index);
  control->SetValueNormalizedFromHost(value);
  if (FBParamTypeIsStepped(control->Param()->static_.type))
    SteppedParamNormalizedChanged(index);
}

FBParamControl*
FBPlugGUI::GetControlForParamIndex(int paramIndex) const
{
  auto iter = _paramIndexToComponent.find(paramIndex);
  assert(iter != _paramIndexToComponent.end());
  return &dynamic_cast<FBParamControl&>(*_store[iter->second].get());
}

std::string
FBPlugGUI::GetParamActiveTooltip(
  FBStaticParam const& param, bool active, float value) const
{
  if (!active)
    return "N/A";
  return param.NormalizedToText(FBParamTextDisplay::TooltipWithUnit, value);
}

void
FBPlugGUI::UpdateExchangeStateTick()
{
  auto const& params = HostContext()->Topo()->params;
  for (int i = 0; i < params.size(); i++)
    if (!FBParamTypeIsStepped(params[i].static_.type))
      dynamic_cast<FBParamSlider&>(*GetControlForParamIndex(i)).UpdateExchangeState();
}

void
FBPlugGUI::ShowPopupMenuFor(
  Component* target,
  PopupMenu menu,
  std::function<void(int)> callback)
{
  PopupMenu::Options options;
  options = options.withParentComponent(this);
  options = options.withTargetComponent(target);
  options = options.withMousePosition();
  menu.showMenuAsync(options, callback);
}

void
FBPlugGUI::UpdateExchangeState()
{
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  auto now = high_resolution_clock::now();
  auto elapsedMillis = duration_cast<milliseconds>(now - _exchangeUpdated);
  if (elapsedMillis.count() < 1000.0 / FBGUIFPS)
    return;
  _exchangeUpdated = now;
  UpdateExchangeStateTick();
}

void
FBPlugGUI::ShowHostMenuForParam(int index)
{
  auto menuItems = HostContext()->MakeParamContextMenu(index);
  if (menuItems.empty())
    return;
  auto hostMenu = FBMakeHostContextMenu(menuItems);
  auto clicked = [this, index](int tag) {
    if (tag > 0)
      HostContext()->ParamContextMenuClicked(index, tag); };
  ShowPopupMenuFor(this, *hostMenu, clicked);
}

Component*
FBPlugGUI::StoreComponent(std::unique_ptr<Component>&& component)
{
  FBParamControl* paramControl;
  FBParamsDependent* paramsDependent;
  Component* result = component.get();
  int componentIndex = (int)_store.size();
  _store.emplace_back(std::move(component));
  if ((paramControl = dynamic_cast<FBParamControl*>(result)) != nullptr)
    _paramIndexToComponent[paramControl->Param()->runtimeParamIndex] = componentIndex;
  if ((paramsDependent = dynamic_cast<FBParamsDependent*>(result)) != nullptr)
    for (int p : paramsDependent->RuntimeDependencies())
      _paramsDependents[p].insert(paramsDependent);
  return result;
}

std::string
FBPlugGUI::GetTooltipForParam(int index) const
{
  auto const& param = HostContext()->Topo()->params[index];
  float normalized = HostContext()->GetParamNormalized(index);
  auto paramActive = HostContext()->ExchangeState()->GetParamActiveState(&param);

  auto result = param.tooltip + ": ";
  result += param.static_.NormalizedToText(
    FBParamTextDisplay::TooltipWithUnit, normalized);
  if (!param.static_.IsVoice())
    result += "\r\nCurrent engine value: " +
    GetParamActiveTooltip(param.static_, paramActive.active, paramActive.minValue);
  else
  {
    result += "\r\n Min engine value: " +
      GetParamActiveTooltip(param.static_, paramActive.active, paramActive.minValue);
    result += "\r\n Max engine value: " +
      GetParamActiveTooltip(param.static_, paramActive.active, paramActive.maxValue);
  }
  result += "\r\nAutomation: " + param.static_.AutomationTooltip();
  return result;
}