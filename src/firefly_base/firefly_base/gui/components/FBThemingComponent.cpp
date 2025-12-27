#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FBThemedComponent::
FBThemedComponent(FBRuntimeTopo const* topo, int componentId, Component* content):
Component(),
_componentId(componentId),
_content(content),
_topo(topo)
{
  addAndMakeVisible(content);
}

void 
FBThemedComponent::paint(Graphics& g)
{
  g.fillAll(FBGetLookAndFeel()->FindColorSchemeFor(*this).background);
}

void
FBThemedComponent::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

int
FBThemedComponent::FixedHeight() const
{
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedHeight() : 0;
}
 
int
FBThemedComponent::FixedWidth(int height) const
{
  auto sizingChild = dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedWidth(height): 0;
}

FBColorScheme const*
FBThemedComponent::GetScheme(FBTheme const& theme) const
{
  auto componentIter = _topo->static_->themedComponents.find(ComponentId());
  if (componentIter != _topo->static_->themedComponents.end())
  {
    auto schemeIter = theme.componentColors.find(FBCleanTopoId(componentIter->second.id));
    if (schemeIter != theme.componentColors.end())
      return &theme.colorSchemes.at(schemeIter->second.colorScheme);
  }
  return nullptr;
}

FBModuleComponent::
FBModuleComponent(
  FBRuntimeTopo const* topo) :
Component(),
_topo(topo) {}

FBModuleComponent::
FBModuleComponent(
  FBRuntimeTopo const* topo, 
  int moduleIndex, int moduleSlot, 
  Component* content):
Component(),
_moduleSlot(moduleSlot),
_moduleIndex(moduleIndex),
_content(content),
_topo(topo)
{
  addAndMakeVisible(content);
}

FBModuleComponent::
FBModuleComponent(
  FBRuntimeTopo const* topo, 
  int moduleIndex, int moduleSlot, 
  std::function<bool(FBTheme const&)> const& followModule, 
  juce::Component* content):
Component(),
_moduleSlot(moduleSlot),
_moduleIndex(moduleIndex),
_content(content),
_topo(topo),
_followModule(followModule)
{
  addAndMakeVisible(content);
}

void 
FBModuleComponent::paint(Graphics& g)
{
  if(_followModule != nullptr && _followModule(FBGetLookAndFeel()->Theme()))
    g.fillAll(FBGetLookAndFeel()->FindColorSchemeFor(*this).background);
}

void
FBModuleComponent::resized()
{
  if (_content != nullptr)
  {
    getChildComponent(0)->setBounds(getLocalBounds());
    getChildComponent(0)->resized();
  }
}

void 
FBModuleComponent::SetModuleContent(int moduleIndex, int moduleSlot, Component* content)
{
  if (_content != nullptr)
    removeChildComponent(0);
  _moduleSlot = moduleSlot;
  _moduleIndex = moduleIndex;
  _content = content;

  if(_content != nullptr)
    addAndMakeVisible(content);

  // need explicit repaint because module index/slot changes how the L&F reacts
  repaint();
}

int
FBModuleComponent::FixedHeight() const
{
  if (_content == nullptr)
    return 0;
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedHeight() : 0;
}

int
FBModuleComponent::FixedWidth(int height) const
{
  if (_content == nullptr)
    return 0;
  auto sizingChild = dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedWidth(height) : 0;
}

FBColorScheme const*
FBModuleComponent::GetScheme(FBTheme const& theme) const
{
  if (_followModule != nullptr && !_followModule(theme))
    return nullptr;
  int rtModuleIndex = _topo->moduleTopoToRuntime.at({ ModuleIndex(), ModuleSlot() });
  auto moduleIter = theme.moduleColors.find(rtModuleIndex);
  if (moduleIter != theme.moduleColors.end())
    return &theme.colorSchemes.at(moduleIter->second.colorScheme);
  return nullptr;
}