#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamToggleButton final:
public juce::ToggleButton,
public IFBParamControl
{
  bool _isOn = {};
  FBRuntimeParam const* const _param;
  IFBHostGUIContext* const _context;

public:
  FBParamToggleButton(
    FBRuntimeParam const* param,
    IFBHostGUIContext* context);

  void buttonStateChanged() override;
  void SetValueNormalized(float normalized) override;
};