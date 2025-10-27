#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FFGMixParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFGMixParamListener();
  FFGMixParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGMixParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeGMixGUITab(FBPlugGUI* plugGUI);
bool FFGMixAdjustParamModulationGUIBounds(
FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm);