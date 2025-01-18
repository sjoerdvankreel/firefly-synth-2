#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>

class IFBParamControl;

class FBPlugGUI:
public juce::Component,
public IFBGUIStore
{
  std::map<int, int> _paramIndexToControl = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};

protected:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugGUI);

public:
  IFBParamControl* 
  GetParamControlForIndex(int paramIndex) const override;
  juce::Component* 
  StoreComponent(std::unique_ptr<juce::Component>&& component) override;
  IFBParamControl* 
  StoreParamControl(int index, std::unique_ptr<IFBParamControl>&& control) override;
};