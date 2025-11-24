#pragma once

#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBMSEGModel.hpp>
#include <firefly_base/gui/components/FBMSEGCanvas.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;

class FBMSEGEditor :
public juce::Component
{
  std::vector<int> const _snapXCounts;
  std::vector<int> const _snapYCounts;
  
  std::unique_ptr<FBMSEGCanvas> _canvas = {};
  std::unique_ptr<FBGridComponent> _grid = {};
  std::unique_ptr<FBGridComponent> _controlGrid = {};
  std::unique_ptr<FBFillerComponent> _controlFiller = {};

  std::unique_ptr<FBAutoSizeLabel> _editXLabel = {};
  std::unique_ptr<FBAutoSizeLabel> _editYLabel = {};
  std::unique_ptr<FBAutoSizeComboBox> _xEditModeCombo = {};
  std::unique_ptr<FBAutoSizeComboBox> _yEditModeCombo = {};
  std::unique_ptr<FBAutoSizeComboBox> _snapXCountCombo = {};
  std::unique_ptr<FBAutoSizeComboBox> _snapYCountCombo = {};

  void ModelUpdated();

public:
  FBMSEGEditor(
    FBPlugGUI* plugGUI,
    std::string const& name,
    int maxPoints,
    double maxLengthTime,
    FBBarsItem const& maxLengthBars,
    std::vector<FBBarsItem> const& allowedBars,
    std::vector<int> const& snapXCounts,
    std::vector<int> const& snapYCounts);

  void resized() override;
  void UpdateModel(); // call after mutating Model()
  FBMSEGModel& Model() { return _canvas->Model(); }

  std::function<void(FBMSEGModel const&)> modelUpdated = {};
  std::function<std::string(FBMSEGNearestHitType hitType, int index)> getTooltipFor = {};
};