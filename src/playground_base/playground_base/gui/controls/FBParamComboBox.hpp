#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamComboBox final:
public juce::ComboBox,
public FBParamControl,
public IFBHorizontalAutoSize
{
  IFBHostGUIContext* const _context;

public:
  FBParamComboBox(
    FBRuntimeParam const* param,
    IFBHostGUIContext* context);

  int FixedWidth() const override;
  void valueChanged(juce::Value& value) override;
  void SetValueNormalized(float normalized) override;
};