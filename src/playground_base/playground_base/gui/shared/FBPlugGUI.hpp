#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>

class IFBParamControl;

class FBPlugGUI:
public juce::Component
{
  std::map<int, int> _paramIndexToControl = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};

protected:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugGUI);

public:
  IFBParamControl* 
  GetParamControlForIndex(int paramIndex) const;
  juce::Component* 
  AddComponent(std::unique_ptr<juce::Component>&& component);
  IFBParamControl* 
  AddParamControl(int index, std::unique_ptr<IFBParamControl>&& control);
};