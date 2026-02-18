#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBPlugGUI;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

enum class FBLabelAlign
{
  Left,
  Right,
  Center
};

enum class FBLabelColors
{
  Regular,
  PrimaryForeground,
  PrimaryBackground
};

class FBAutoSizeLabel:
public juce::Label,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  FBPlugGUI* const _plugGUI;
  std::string const _text;
  FBLabelColors const _colors;
public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBLabelColors Colors() const { return _colors; }
  FBAutoSizeLabel(FBPlugGUI* plugGUI, std::string const& text, FBLabelAlign align = {}, FBLabelColors colors = {});
};

class FBAutoSizeLabel2:
public juce::Label,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  bool const _hasBackground;
  int const _fixedWidth;

public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  FBAutoSizeLabel2(bool hasBackground, int fixedWidth);
  bool HasBackground() const { return _hasBackground; }
};

class FBGUIParamLabel final:
public FBAutoSizeLabel,
public FBGUIParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed = true, FBLabelColors colors = {});
};

class FBParamLabel final:
public FBAutoSizeLabel,
public FBParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed = true, FBLabelColors colors = {});
};

class FBParamLinkedLabel final:
public FBAutoSizeLabel,
public FBParamsDependent
{
  std::string const _text;
public:
  void parentHierarchyChanged() override;
  FBParamLinkedLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string text, FBLabelColors colors = {});
};