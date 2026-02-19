#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/shared/FBPlugGUIListeners.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

// Displays the actual param value.
class FBParamDisplayLabel final:
public juce::Label,
public FBParamControl,
public IFBHorizontalAutoSize
{
  std::string const _maxWidthText;

public:
  FBParamDisplayLabel(
    FBPlugGUI* plugGUI,
    FBRuntimeParam const* param,
    std::string const& maxWidthText,
    bool isThemed = true);

  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  void SetValueNormalizedFromHost(double normalized) override;
};

// Displays custom text if param value is not false (requires boolean/toggle).
class FBMultiParamDisplayLabel final:
public juce::Label,
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;
  std::vector<FBRuntimeParam const*> _params;
  std::vector<std::string> const _texts;

  void UpdateText();

public:
  ~FBMultiParamDisplayLabel();
  FBMultiParamDisplayLabel(
    FBPlugGUI* plugGUI,
    std::vector<FBRuntimeParam const*> const& params,
    std::vector<std::string> const& texts);
  
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
