#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBImageComponent.hpp>

using namespace juce;

FBImageComponent::
FBImageComponent(
  int width,
  std::string const& resourceName, 
  juce::RectanglePlacement placement):
_width(width),
_resourceName(resourceName)
{
  _image = std::make_unique<ImageComponent>();
  _image->setImage(ImageCache::getFromFile(String(GetCurrentImagePath())), placement);
  addAndMakeVisible(_image.get());
}

std::string 
FBImageComponent::GetCurrentImagePath() const
{
  auto folderName = FBGetLookAndFeel()->Theme().folderName;
  String path((FBGetResourcesFolderPath() / "ui" / "themes" / folderName / _resourceName).string());
  return path.toStdString();
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