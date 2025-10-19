#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FBPlugGUI;
class FBGraphRenderState;
class FBModuleGraphComponent;

class FFGlobalUniParamListener:
public IFBParamListener
{
  FBPlugGUI* const _plugGUI;

public:
  ~FFGlobalUniParamListener();
  FFGlobalUniParamListener(FBPlugGUI* plugGUI);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGlobalUniParamListener);
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};

void 
FFGlobalUniInit(FBPlugGUI* plugGUI);
juce::Component* 
FFMakeGlobalUniEditorHeader(FBPlugGUI* plugGUI);
juce::Component*
FFMakeGlobalUniEditorContent(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState,
  std::vector<FBModuleGraphComponent*>* fixedGraphs);
