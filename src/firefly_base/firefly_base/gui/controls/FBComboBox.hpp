#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

class FBPlugGUI;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

class FBAutoSizeComboBox:
public juce::ComboBox,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int _maxTextWidth = 0;

public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  virtual void OnPopupMenuClosing(int /*itemResultId*/) {}
  FBAutoSizeComboBox(juce::PopupMenu const& rootMenu);
};

class FBGUIParamComboBox final:
public FBAutoSizeComboBox,
public FBGUIParamControl
{
public:
  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  void valueChanged(juce::Value& value) override;

  void SetValueNormalizedFromPlug(double normalized) override;
  FBGUIParamComboBox(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};

class FBParamComboBox final:
public FBAutoSizeComboBox,
public FBParamControl
{
  void UpdateDependentComboboxTarget();
  void EnableDependentItems(juce::PopupMenu* menu, int runtimeSourceValue);

public:
  // Well this is kind of lame but for the time being, i really 
  // don't need a full-blown register/deregister callback mechanism.
  std::function<void(int itemResultId)> valueChangedByUserAction = {};

  void showPopup() override;
  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  void valueChanged(juce::Value& value) override;

  void OnPopupMenuClosing(int itemResultId) override;
  void SetValueNormalizedFromHost(double normalized) override;
  FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};