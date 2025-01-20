#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamToggleButton final:
public juce::ToggleButton,
public FBParamControl,
public IFBHorizontalAutoSize
{
  bool _isOn = {};

public:
  FBParamToggleButton(
    FBRuntimeTopo const* topo,
    FBRuntimeParam const* param,
    IFBHostGUIContext* hostContext);

  void buttonStateChanged() override;
  int FixedWidth(int height) const override;
  void SetValueNormalized(float normalized) override;
};