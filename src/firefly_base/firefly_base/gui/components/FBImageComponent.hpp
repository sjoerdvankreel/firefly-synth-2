#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <memory>

class FBPlugGUI;

class FBImageComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBThemeListener
{
  FBPlugGUI* const _plugGUI;
  int const _width;
  std::string const _resourceName;
  juce::RectanglePlacement const _placement;
  std::unique_ptr<juce::ImageComponent> _image = {};

  std::string GetCurrentImagePath() const;

public:
  ~FBImageComponent();
  FBImageComponent(
    FBPlugGUI* plugGUI, int width,
    std::string const& resourceName, 
    juce::RectanglePlacement placement);

  void resized() override;
  void ThemeChanged() override;
  int FixedWidth(int height) const override;
};