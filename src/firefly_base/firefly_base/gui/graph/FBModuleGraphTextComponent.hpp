#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <chrono>
#include <vector>

class FBPlugGUI;
struct FBModuleGraphComponentData;

enum class FBModuleGraphTextType
{
  TitleGain,
  Main,
  Both
};

class FBModuleGraphTextComponent:
public juce::Component,
public IFBVerticalAutoSize
{
  FBPlugGUI* const _plugGUI;
  FBModuleGraphComponentData const* const _data;
  int const _graphIndex;
  FBModuleGraphTextType _textType;

  std::string _mainText = {};
  std::string _titleAndGainText = {};
  std::vector<float> _gainSlidingWindow = {};
  std::chrono::high_resolution_clock::time_point _updated = {};

public:

  FBModuleGraphTextComponent(
    FBPlugGUI* plugGUI, 
    FBModuleGraphComponentData const* data, 
    int graphIndex,
    FBModuleGraphTextType textType);

  int FixedHeight() const override;
  void paint(juce::Graphics& g) override;
};
