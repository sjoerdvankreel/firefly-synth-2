#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBInstanceNameEditor:
public juce::TextEditor,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize,
public juce::TextEditor::Listener
{
  int const _fixedWidth;
  FBPlugGUI* const _plugGUI;

public:
  FBInstanceNameEditor(FBPlugGUI* plugGUI, int fixedWidth);

  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  void paint(juce::Graphics& g) override;
  void textEditorTextChanged(juce::TextEditor&) override;
  void textEditorFocusLost(juce::TextEditor&) override {}
  void textEditorReturnKeyPressed(juce::TextEditor&) override {}
  void textEditorEscapeKeyPressed(juce::TextEditor&) override {}
};