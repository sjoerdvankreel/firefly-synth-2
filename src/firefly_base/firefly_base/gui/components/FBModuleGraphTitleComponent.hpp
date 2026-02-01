#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBModuleGraphComponentData;

class FBModuleGraphTitleComponent final:
public juce::Component,
public IFBVerticalAutoSize
{
  FBPlugGUI* const _plugGUI;
  FBModuleGraphComponentData const* const _data;
  int const _graphIndex;

  std::string _mainText = {};
  std::string _titleAndSubText = {};
  std::chrono::high_resolution_clock::time_point _updated = {};

public:
  int FixedHeight() const override;
  void paint(juce::Graphics& g) override;
  FBModuleGraphTitleComponent(FBPlugGUI* plugGUI, FBModuleGraphComponentData const* data, int graphIndex);
};