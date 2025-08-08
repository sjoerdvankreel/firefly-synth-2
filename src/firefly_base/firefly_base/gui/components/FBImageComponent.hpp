#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <memory>

class FBPlugGUI;

class FBImageComponent:
public juce::Component,
public IFBHorizontalAutoSize
{
  int const _width;
  FBPlugGUI* const _plugGUI;
  std::unique_ptr<juce::ImageComponent> _image = {};

public:
  FBImageComponent(
    FBPlugGUI* plugGUI, 
    int width,
    std::string const& resourceName, 
    juce::RectanglePlacement placement);

  void resized() override;
  int FixedWidth(int height) const override;
};