#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <vector>

using namespace juce;

static const std::vector<std::string> NoteNames = 
{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

int 
FBNoteParamNonRealTime::ValueCount() const
{
  return MidiNoteCount;
}

double
FBNoteParamNonRealTime::PlainToNormalized(double plain) const
{
  return std::clamp(plain / (ValueCount() - 1.0), 0.0, 1.0);
}

double
FBNoteParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return FBNoteParamRealTime::NormalizedToPlain((float)normalized);
}

std::string
FBNoteParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const
{
  int plainDiscrete = (int)std::round(plain);
  return NoteNames[plainDiscrete % 12] + std::to_string(plainDiscrete / 12 - 1);
}

std::optional<double>
FBNoteParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  for (int i = 0; i < ValueCount(); i++)
    if (text == PlainToText(display, i))
      return { i };
  return {};
}

PopupMenu
FBNoteParamNonRealTime::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < NoteNames.size(); i++)
  {
    PopupMenu noteMenu;
    for (int j = 0; j < 128.0f / 12.0f; j++)
    {
      int midiNote = j * 12 + i;
      if (midiNote < 128)
        noteMenu.addItem(midiNote + 1, PlainToText(FBValueTextDisplay::Text, midiNote));
    }
    result.addSubMenu(NoteNames[i], noteMenu);
  }
  return result;
}