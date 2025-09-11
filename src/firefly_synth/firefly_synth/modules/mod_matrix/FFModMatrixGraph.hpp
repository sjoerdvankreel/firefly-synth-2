#pragma once

#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FFPlugGUI;
struct FBRuntimeParam;

enum class FFModMatrixGraphType {
  Source, SourceLowHigh, Scale, 
  SourceScaled, Target, TargetModulated,
  Count };

// Custom graph, not tied to audio engine state.
// If it was, we'd need to basically calculate the entire synth to plot this.
// So instead, provide some dummy data and show how the knobs change it.
class FFModMatrixGraph:
public juce::Component,
public IFBParamListener
{
  FFPlugGUI* const _plugGUI;
  FFModMatrixGraphType const _type;
  FBRuntimeParam const* _trackingParam = nullptr;

public:
  ~FFModMatrixGraph();
  FFModMatrixGraph(FFPlugGUI* plugGUI, FFModMatrixGraphType type);
  
  void paint(juce::Graphics& g) override;
  void AudioParamChanged(int index, double normalized, bool changedFromUI) override;
};