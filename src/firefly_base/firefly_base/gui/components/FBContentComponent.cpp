#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>

using namespace juce;

void
FBContentComponent::resized()
{
  auto child = getChildComponent(0);
  if (child)
  {
    child->setBounds(getLocalBounds());
    child->resized();
  }
}

void
FBContentComponent::SetContent(Component* content)
{
  removeAllChildren();
  addAndMakeVisible(content);
  resized();
}

int
FBContentComponent::FixedHeight() const
{
  return dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0))->FixedHeight();
}

int
FBContentComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height);
}

FBMultiContentComponent::
FBMultiContentComponent()
{
  addAndMakeVisible(_component);
}

void 
FBMultiContentComponent::resized()
{
  _component.setBounds(getLocalBounds());
  _component.resized();
}

int
FBMultiContentComponent::FixedHeight() const
{
  return _component.FixedHeight();
}

int
FBMultiContentComponent::FixedWidth(int height) const
{
  return _component.FixedWidth(height);
}

void
FBMultiContentComponent::SelectContentIndex(int index)
{
  index = std::clamp(index, 0, (int)_content.size() - 1);
  _component.SetContent(_content[index]);
  resized();
}

void 
FBMultiContentComponent::SetContent(int index, Component* content)
{
  if (index >= _content.size())
    _content.resize(index + 1);
  _content[index] = content;
}