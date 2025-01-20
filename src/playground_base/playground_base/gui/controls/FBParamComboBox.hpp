#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamComboBox final:
public juce::ComboBox,
public FBParamControl,
public IFBHorizontalAutoSize
{
public:
  FBParamComboBox(
    FBRuntimeTopo const* topo,
    FBRuntimeParam const* param,
    IFBHostGUIContext* hostContext);

  int FixedWidth(int height) const override;
  void valueChanged(juce::Value& value) override;
  void SetValueNormalized(float normalized) override;
};