#pragma once

#include <firefly_base/gui/shared/FBPlugGUIListeners.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FFPlugGUI;
class FBHostGUIContext;

class FFModMatrixParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFModMatrixParamListener();
  FFModMatrixParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFModMatrixParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeModMatrixGUI(FFPlugGUI* plugGUI);
bool FFModMatrixAdjustParamModulationGUIBounds(
FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm);