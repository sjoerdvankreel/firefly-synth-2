#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBImageComponent.hpp>

using namespace juce;

FBImageComponent::
~FBImageComponent()
{
  _plugGUI->RemoveThemeListener(this);
}

FBImageComponent::
FBImageComponent(
  FBPlugGUI* plugGUI, int width,
  std::string const& resourceName, 
  RectanglePlacement placement):
_plugGUI(plugGUI),
_width(width),
_resourceName(resourceName),
_placement(placement)
{
  _plugGUI->AddThemeListener(this);
  _image = std::make_unique<ImageComponent>();
  _image->setImage(ImageCache::getFromFile(String(GetCurrentImagePath())), placement);
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

void 
FBImageComponent::ThemeChanged()
{
  _image->setImage(ImageCache::getFromFile(String(GetCurrentImagePath())), _placement);
}

std::string 
FBImageComponent::GetCurrentImagePath() const
{
  auto folderName = FBGetLookAndFeel()->Theme().folderName;
  String path((FBGetResourcesFolderPath() / "ui" / "themes" / folderName / _resourceName).string());
  FB_ASSERT(juce::File(String(path)).existsAsFile());
  return path.toStdString();
}