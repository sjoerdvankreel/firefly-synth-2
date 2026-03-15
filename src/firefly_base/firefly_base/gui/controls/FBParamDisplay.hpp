#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/shared/FBPlugGUIListeners.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamDisplayLabel final:
public juce::Label,
public FBParamControl,
public IFBHorizontalAutoSize
{
  std::string _maxWidthText = {};

public:
  FBParamDisplayLabel(
    FBPlugGUI* plugGUI,
    FBRuntimeParam const* param,
    bool isThemed = true);

  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  void SetValueNormalizedFromHost(double normalized) override;
};

class FBMultiParamDisplayLabel final:
public juce::Label,
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;
  std::vector<FBRuntimeParam const*> _params;
  std::function<std::string(std::vector<double> const&)> _normalizedToText;
  std::vector<double> _normalized = {};

  void UpdateText();

public:
  ~FBMultiParamDisplayLabel();
  FBMultiParamDisplayLabel(
    FBPlugGUI* plugGUI,
    std::vector<FBRuntimeParam const*> const& params,
    std::function<std::string(std::vector<double> const&)> normalizedToText);
  
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

class FBParamDisplayMeter final:
public juce::Component,
public FBParamControl
{
  int _fillCount = 0;

public:
  FBParamDisplayMeter(
    FBPlugGUI* plugGUI,
    FBRuntimeParam const* param,
    bool isThemed = true);

  void paint(juce::Graphics& g) override;
  void parentHierarchyChanged() override;
  void SetValueNormalizedFromHost(double normalized) override;
};
