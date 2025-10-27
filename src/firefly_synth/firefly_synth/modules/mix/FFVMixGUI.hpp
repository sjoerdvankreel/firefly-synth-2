#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FFVMixParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFVMixParamListener();
  FFVMixParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVMixParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeVMixGUITab(FBPlugGUI* plugGUI);
bool FFVMixAdjustParamModulationGUIBounds(
FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm);