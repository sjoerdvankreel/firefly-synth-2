#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamToggleButton final:
public juce::ToggleButton,
public FBParamControl,
public IFBHorizontalAutoSize
{
  bool _isOn = {};
  IFBHostGUIContext* const _context;

public:
  FBParamToggleButton(
    FBRuntimeParam const* param,
    IFBHostGUIContext* context);

  void buttonStateChanged() override;
  int FixedWidth(int height) const override;
  void SetValueNormalized(float normalized) override;
};