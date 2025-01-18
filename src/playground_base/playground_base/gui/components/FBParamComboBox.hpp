#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamComboBox final:
public juce::ComboBox,
public IFBParamControl
{
  FBRuntimeParam const* const _param;
  IFBHostGUIContext* const _context;

public:
  FBParamComboBox(
    FBRuntimeParam const* param,
    IFBHostGUIContext* context);

  void valueChanged(juce::Value& value) override;
  void SetValueNormalized(float normalized) override;
};