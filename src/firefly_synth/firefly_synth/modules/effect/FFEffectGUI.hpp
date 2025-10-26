#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FFEffectParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFEffectParamListener();
  FFEffectParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeEffectGUI(FBPlugGUI* plugGUI);
bool FFEffectAdjustParamModulationGUIBounds(
FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm);