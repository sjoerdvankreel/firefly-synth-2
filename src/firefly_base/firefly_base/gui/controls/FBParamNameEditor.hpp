#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBParamNameEditor:
public juce::TextEditor,
public juce::TextEditor::Listener
{
  int _paramIndex = -1;
  FBPlugGUI* const _plugGUI;
  LengthAndCharacterRestriction _filter;

public:
  FBParamNameEditor(FBPlugGUI* plugGUI, int fixedWidth);

  void InitEdit(int paramIndex);
  void paint(juce::Graphics& g) override;
  void textEditorTextChanged(juce::TextEditor&) override;
  void textEditorFocusLost(juce::TextEditor&) override {}
  void textEditorReturnKeyPressed(juce::TextEditor&) override {}
  void textEditorEscapeKeyPressed(juce::TextEditor&) override {}
};