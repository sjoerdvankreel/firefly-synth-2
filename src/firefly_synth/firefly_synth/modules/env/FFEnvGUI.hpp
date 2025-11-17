#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/components/FBMSEGEditor.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FFEnvParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;
  std::vector<FBMSEGEditor*> const _msegEditors;

public:
  ~FFEnvParamListener();
  FFEnvParamListener(FBPlugGUI* plugGUI, std::vector<FBMSEGEditor*> const& msegEditors);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEnvParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

juce::Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI, std::vector<FBMSEGEditor*>& msegEditors);