#pragma once

#include <firefly_base/gui/shared/FBPlugGUIListeners.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FFVoiceModuleParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFVoiceModuleParamListener();
  FFVoiceModuleParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVoiceModuleParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeVoiceModuleGUI(FBPlugGUI* plugGUI);
bool FFVoiceModuleAdjustParamModulationGUIBounds(
FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm);
