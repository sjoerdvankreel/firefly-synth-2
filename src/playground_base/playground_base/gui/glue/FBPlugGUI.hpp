#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class IFBHostGUIContext;

class FBPlugGUI:
public juce::Component  
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);
  virtual void SetParamNormalized(int index, float normalized) = 0;
protected:
  IFBHostGUIContext* const _hostContext;
  FBPlugGUI(IFBHostGUIContext* hostContext) : _hostContext(hostContext) {}
};