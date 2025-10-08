#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <functional>

class FBPlugGUI;
struct FBRuntimeParam;

class FBAutoSizeButton:
public juce::TextButton,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _textWidth;
public:
  FBAutoSizeButton(std::string const& text);
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};

class FBParamLinkedButton final:
public FBAutoSizeButton,
public FBParamsDependent
{
public:
  void parentHierarchyChanged() override;
  FBParamLinkedButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string const& text);
};

class FBParamValueLinkedButton final:
public FBAutoSizeButton,
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;
  FBRuntimeParam const* _param;
  std::function<bool(int)> _enabledIf;
public:
  ~FBParamValueLinkedButton();
  FBParamValueLinkedButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string const& text, std::function<bool(int)> enabledIf);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};