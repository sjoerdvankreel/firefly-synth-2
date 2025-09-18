#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

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