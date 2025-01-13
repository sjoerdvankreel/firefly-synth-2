#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/components/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBStaticGUITopo;

class FBPlugGUIBase:
public juce::Component,
public FBPlugGUIContext
{
protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIBase);
  FBPlugGUIBase(FBStaticGUITopo const* topo);
  virtual IFBParamControl* 
  GetParamControlForIndex(int index) = 0;

public:
  void paint(juce::Graphics& g) override;
  void RemoveFromDesktop() override;
  void AddToDesktop(void* parent) override;
  void SetVisible(bool visible) override;
  void SetParamNormalized(int index, float normalized) override;
};