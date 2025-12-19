#include <firefly_base/gui/components/FBModuleComponent.hpp>

using namespace juce;

FBModuleComponent::
FBModuleComponent(int moduleIndex, int moduleSlot, Component* content):
Component(),
_moduleSlot(moduleSlot),
_moduleIndex(moduleIndex),
_content(content)
{
  addAndMakeVisible(content);
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