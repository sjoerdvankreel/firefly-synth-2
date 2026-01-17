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
  FBPlugGUI* plugGUI, FBThemeResourceId resourceId,
  int width, RectanglePlacement placement):
_plugGUI(plugGUI),
_resourceId(resourceId),
_width(width),
_placement(placement)
{
  _plugGUI->AddThemeListener(this);
  _image = std::make_unique<ImageComponent>();
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
  auto const& theme = FBGetLookAndFeelFor(_plugGUI)->Theme();
  auto resourceName = theme.global.resources.GetResourceName(_resourceId);
  auto folderName = theme.global.resources.GetResourceName(FBThemeResourceId::FolderName);
  String path((FBGetThemesFolderPath() / folderName / resourceName).string());
  FB_ASSERT(juce::File(String(path)).existsAsFile());
  return path.toStdString();
}