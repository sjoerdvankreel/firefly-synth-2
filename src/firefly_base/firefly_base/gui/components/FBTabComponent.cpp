#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <map>
#include <string>

using namespace juce;

FBModuleSelector::
FBModuleSelector(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
_plugGUI(plugGUI), _param(param)
{
  double normalized = _plugGUI->HostContext()->GetGUIParamNormalized(_param->runtimeParamIndex);
  _storedSelection = _param->static_.Discrete().NormalizedToPlainFast((float)normalized);
}

void 
FBModuleSelector::SelectModuleGUI(int index)
{
  auto const& indices = _moduleIndices[index];
  _plugGUI->ActiveModuleSlotChanged(indices.index, indices.slot);
  double normalized = _param->static_.Discrete().PlainToNormalizedFast(index);
  _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
  _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
}

void 
FBModuleSelector::ShowModulePopupMenuFor(int index, Component* clicked)
{
  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto moduleIndices = _moduleIndices[index];
  auto const& staticTopo = *_plugGUI->HostContext()->Topo()->static_;
  auto const& staticModule = staticTopo.modules[moduleIndices.index];

  PopupMenu menu;
  menu.addItem(1, "Clear");
  if (staticModule.slotCount > 1)
  {
    PopupMenu subMenu;
    for (int i = 0; i < staticModule.slotCount; i++)
    {
      auto name = FBMakeRuntimeModuleShortName(
        staticTopo, staticModule.name, staticModule.slotCount, i, 
        staticModule.slotFormatter, staticModule.slotFormatterOverrides);
      subMenu.addItem(2 + i, name, moduleIndices.slot != i);
    }
    menu.addSubMenu("Copy To", subMenu);
  }
  auto iter = PopupMenu::MenuItemIterator(extendedMenu);
  while (iter.next())
    menu.addItem(iter.getItem());

  PopupMenu::Options options;
  options = options.withParentComponent(_plugGUI);
  options = options.withStandardItemHeight(lnf->GetStandardPopupMenuItemHeight());
  options = options.withTargetComponent(clicked);
  menu.showMenuAsync(options, [this, moduleIndices, slotCount = staticModule.slotCount](int id) {
    std::string name = _plugGUI->HostContext()->Topo()->ModuleAtTopo(moduleIndices)->name;
    if (id == 1)
    {
      _plugGUI->HostContext()->UndoState().Snapshot("Clear " + name);
      _plugGUI->HostContext()->ClearModuleAudioParams(moduleIndices);
    }
    else if (2 <= id && id < slotCount + 2)
    {
      _plugGUI->HostContext()->UndoState().Snapshot("Copy " + name);
      _plugGUI->HostContext()->CopyModuleAudioParams(moduleIndices, id - 2);
    }
    else if (extendedMenuHandler)
    {
      extendedMenuHandler(_plugGUI, moduleIndices, id);
    }
  });
}

FBTabBarButton::
FBTabBarButton(const String& name, TabbedButtonBar& bar):
TabBarButton(name, bar) {}

void 
FBTabBarButton::clicked(const ModifierKeys& modifiers)
{
  FBModuleTabComponent* tabs;
  if (!modifiers.isRightButtonDown())
    TabBarButton::clicked(modifiers);
  else if ((tabs = findParentComponentOfClass<FBModuleTabComponent>()) != nullptr)
    tabs->TabRightClicked(getIndex());
}

FBModuleTabBarButton::
FBModuleTabBarButton(
FBPlugGUI* plugGUI,
std::string const& separatorText,
const juce::String& name,
juce::TabbedButtonBar& bar,
FBTopoIndices const& moduleIndices):
FBTabBarButton(name, bar),
_plugGUI(plugGUI),
_separatorText(separatorText),
_moduleIndices(moduleIndices) {}

void
FBModuleTabBarButton::clicked(const ModifierKeys& modifiers)
{
  FBTabBarButton::clicked(modifiers);
  _plugGUI->ModuleSlotClicked(_moduleIndices.index, _moduleIndices.slot);
}

FBColorScheme const* 
FBModuleTabBarButton::GetScheme(FBTheme const& theme) const
{
  int rtModuleIndex = _plugGUI->HostContext()->Topo()->moduleTopoToRuntime.at(_moduleIndices);
  auto moduleIter = theme.moduleColors.find(rtModuleIndex);
  if (moduleIter != theme.moduleColors.end())
    return &theme.global.colorSchemes.at(moduleIter->second.colorScheme);
  return nullptr;
}

FBAutoSizeTabComponent::
FBAutoSizeTabComponent(FBPlugGUI* plugGUI):
FBAutoSizeTabComponent(plugGUI, false) {}

FBAutoSizeTabComponent::
FBAutoSizeTabComponent(FBPlugGUI* plugGUI, bool big):
TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop),
_tabPlugGUI(plugGUI), _big(big)
{
  setOutline(0);
  setTabBarDepth(_big? FBTabBarDepthBig: FBTabBarDepth);
}

int
FBAutoSizeTabComponent::FixedWidth(int height) const
{
  int fontSize = FBGetLookAndFeelFor(_tabPlugGUI)->Theme().global.fontSize;
  auto& content = dynamic_cast<IFBHorizontalAutoSize&>(*getTabContentComponent(0));
  return content.FixedWidth(height - fontSize + 2) + 2;
}

TabBarButton*
FBAutoSizeTabComponent::createTabButton(const juce::String& tabName, int /*tabIndex*/)
{
  return new FBTabBarButton(tabName, *tabs);
}

void
FBAutoSizeTabComponent::currentTabChanged(int /*newCurrentTabIndex*/, const String& /*newCurrentTabName*/)
{
  _tabPlugGUI->HideAllOverlaysAndFileBrowsers();
}

void
FBAutoSizeTabComponent::AddTab(
  std::string const& header, bool centerText, Component* component)
{
  addTab(header, Colours::transparentBlack, component, false);
  auto button = getTabbedButtonBar().getTabButton(getTabbedButtonBar().getNumTabs() - 1);
  auto& fbTabButton = dynamic_cast<FBTabBarButton&>(*button);
  fbTabButton.centerText = centerText;
}

FBModuleTabComponent::
FBModuleTabComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeTabComponent(plugGUI),
FBModuleSelector(plugGUI, param)
{
  assert(param != nullptr);
}

void
FBModuleTabComponent::TabRightClicked(int tabIndex)
{
  if (tabIndex >= 0 && tabIndex < (int)_moduleIndices.size())
    ShowModulePopupMenuFor(tabIndex, getTabbedButtonBar().getTabButton(tabIndex));
}

void 
FBModuleTabComponent::SetTabSeparatorText(int tabIndex, std::string const& text)
{
  _tabSeparatorText[tabIndex] = text;
}

TabBarButton*
FBModuleTabComponent::createTabButton(const juce::String& tabName, int tabIndex)
{
  return new FBModuleTabBarButton(_tabPlugGUI, _tabSeparatorText[tabIndex], tabName, *tabs, _moduleIndices[tabIndex]);
}

void
FBModuleTabComponent::ActivateStoredSelection()
{
  if (0 <= _storedSelection && _storedSelection < _moduleIndices.size())
    setCurrentTabIndex(_storedSelection);
}

void
FBModuleTabComponent::currentTabChanged(
  int newCurrentTabIndex, juce::String const& newCurrentTabName)
{
  FBAutoSizeTabComponent::currentTabChanged(newCurrentTabIndex, newCurrentTabName);
  if (newCurrentTabIndex >= 0 && newCurrentTabIndex < _moduleIndices.size())
    SelectModuleGUI(newCurrentTabIndex);
}

void
FBModuleTabComponent::AddModuleTab(
  bool centerText, bool large,
  FBTopoIndices const& moduleIndices,
  Component* component)
{
  _moduleIndices.push_back(moduleIndices);
  auto topo = _tabPlugGUI->HostContext()->Topo();
  auto const& module = topo->static_->modules[moduleIndices.index];

  std::string header = {};
  if (module.tabSlotFormatter != nullptr)
    header = FBMakeRuntimeModuleShortName(
      *topo->static_, module.name, module.slotCount,
      moduleIndices.slot, module.tabSlotFormatter, module.slotFormatterOverrides);
  else if(module.slotCount > 1)
    header = std::to_string(moduleIndices.slot + 1);
  else
    header = module.name;

  addTab(header, Colours::black, component, false);
  auto button = getTabbedButtonBar().getTabButton(static_cast<int>(_moduleIndices.size() - 1));
  auto& fbTabButton = dynamic_cast<FBTabBarButton&>(*button);
  fbTabButton.large = large;
  fbTabButton.centerText = centerText;
}

FBSelectComponent::
FBSelectComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, std::vector<int> const& rows, std::vector<int> const& cols):
FBModuleSelector(plugGUI, param)
{
  _content = std::make_unique<FBContentComponent>();
  _mainGrid = std::make_unique<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  _selectGrid = std::make_unique<FBGridComponent>(plugGUI, false, rows, cols);
  _mainGrid->Add(0, 0, _selectGrid.get());
  _mainGrid->Add(0, 1, _content.get());
  addAndMakeVisible(_mainGrid.get());
}

void 
FBSelectComponent::resized()
{
  _mainGrid->setBounds(getLocalBounds());
  _mainGrid->resized();
}

void
FBSelectComponent::ActivateStoredSelection()
{
  if (0 <= _storedSelection && _storedSelection < _moduleIndices.size())
    Select(_storedSelection);
}

void 
FBSelectComponent::Select(int index)
{
  for (int i = 0; i < _buttons.size(); i++)
    _buttons[i]->setToggleState(false, dontSendNotification);
  _buttons[index]->setToggleState(true, dontSendNotification);
  _content->SetContent(_components[index]);
  _plugGUI->HideAllOverlaysAndFileBrowsers();
  SelectModuleGUI(index);
}

void 
FBSelectComponent::AddLabel(int row, int col, std::string const& text)
{
  auto label = std::make_unique<FBAutoSizeLabel>(_plugGUI, text);
  _selectGrid->Add(row, col, label.get());
  _labels.push_back(std::move(label));
}

void
FBSelectComponent::AddSelector(int row, int col, FBTopoIndices const& moduleIndices, std::string const& text, juce::Component* component)
{
  // todo menu
  int index = (int)_buttons.size();
  auto button = std::make_unique<FBAutoSizeButton>(_plugGUI, text);
  button->setToggleable(true);
  button->onClick = [this, index] { Select(index); };
  _selectGrid->Add(row, col, button.get());
  _components.push_back(component);
  _buttons.push_back(std::move(button));
  _moduleIndices.push_back(moduleIndices);
  if (_buttons.size() == 1)
    Select(0);
}