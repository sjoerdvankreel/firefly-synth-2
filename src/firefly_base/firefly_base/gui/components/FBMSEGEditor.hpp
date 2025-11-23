#pragma once

#include <firefly_base/gui/components/FBMSEGModel.hpp>
#include <firefly_base/gui/components/FBMSEGCanvas.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;

class FBMSEGEditor :
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  std::unique_ptr<FBMSEGCanvas> _canvas = {};

public:
  FBMSEGEditor(
    FBPlugGUI* plugGUI,
    std::string const& name,
    int maxPoints,
    int maxLengthRatioNum,
    int maxLengthRatioDen,
    double maxLengthReal);

  void resized() override;
  void UpdateModel(); // call after mutating Model()
  FBMSEGModel& Model() { return _canvas->Model(); }

  std::function<void(FBMSEGModel const&)> modelUpdated = {};
  std::function<std::string(FBMSEGNearestHitType hitType, int index)> getTooltipFor = {};
};