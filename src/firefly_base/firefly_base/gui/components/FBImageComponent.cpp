#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/components/FBImageComponent.hpp>

using namespace juce;

FBImageComponent::
FBImageComponent(
  int width,
  std::string const& resourceName, 
  juce::RectanglePlacement placement):
_width(width)
{
  _image = std::make_unique<ImageComponent>();
  String path((FBGUIGetResourcesFolderPath() / resourceName).string());
  _image->setImage(ImageCache::getFromFile(path), placement);
  addAndMakeVisible(_image.get());
}

void 
FBImageComponent::resized()
{
  _image->setBounds(getLocalBounds());
  _image->resized();
}

int 
FBImageComponent::FixedWidth(int) const
{
  return _width;
}