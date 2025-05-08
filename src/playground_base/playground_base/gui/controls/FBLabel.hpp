#pragma once

#include <playground_base/gui/shared/FBVerticalAutoSize.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/gui/shared/FBGUIParamComponent.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

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
  FBAutoSizeLabel(std::string const& text);
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};

class FBGUIParamLabel final:
public FBAutoSizeLabel,
public FBGUIParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};

class FBParamLabel final:
public FBAutoSizeLabel,
public FBParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};