#pragma once

#include <firefly_base/gui/shared/FBPlugGUIListeners.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
class FFPlugGUI;

class FFOsciParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFOsciParamListener();
  FFOsciParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeOsciGUI(FFPlugGUI* plugGUI);
juce::Component*
FFMakeOsciDetailGUI(FFPlugGUI* plugGUI, int moduleSlot);

bool
FFOsciAdjustParamModulationGUIBounds(
  FBHostContext const* ctx, int index,
  float& currentMinNorm, float& currentMaxNorm);