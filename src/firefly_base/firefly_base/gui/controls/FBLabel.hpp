#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBPlugGUI;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

class FBAutoSizeLabel:
public juce::Label,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _textWidth;
public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBAutoSizeLabel(std::string const& text, bool centred = false);
};

class FBAutoSizeLabel2:
public juce::Label,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _fixedWidth;
public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBAutoSizeLabel2(int fixedWidth);
};

class FBGUIParamLabel final:
public FBAutoSizeLabel,
public FBGUIParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed = true);
};

class FBParamLabel final:
public FBAutoSizeLabel,
public FBParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed = true);
};

class FBParamLinkedLabel final:
public FBAutoSizeLabel,
public FBParamsDependent
{
  std::string const _text;
public:
  void parentHierarchyChanged() override;
  FBParamLinkedLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string text);
};