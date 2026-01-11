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
  bool const _isPrimary;
public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  bool IsPrimary() const { return _isPrimary; }
  FBAutoSizeLabel(std::string const& text, bool centred = false, bool isPrimary = false);
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
  FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed = true, bool isPrimary = false);
};

class FBParamLabel final:
public FBAutoSizeLabel,
public FBParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed = true, bool isPrimary = false);
};

class FBParamLinkedLabel final:
public FBAutoSizeLabel,
public FBParamsDependent
{
  std::string const _text;
public:
  void parentHierarchyChanged() override;
  FBParamLinkedLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isPrimary, std::string text);
};