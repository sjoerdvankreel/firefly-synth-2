#include <playground_base/base/topo/param/FBNoteParam.hpp>

#include <vector>

using namespace juce;

static const std::vector<std::string> NoteNames = 
{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

bool
FBNoteParamNonRealTime::IsItems() const
{
  return true;
}

bool 
FBNoteParamNonRealTime::IsStepped() const
{
  return true;
}

int
FBNoteParamNonRealTime::ValueCount() const
{
  return MidiNoteCount;
}

double 
FBNoteParamNonRealTime::PlainToNormalized(double plain) const
{
  return std::clamp(plain / (MidiNoteCount - 1.0), 0.0, 1.0);
}

double
FBNoteParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return std::clamp(static_cast<int>(normalized * MidiNoteCount), 0, MidiNoteCount - 1);
}

std::string
FBNoteParamNonRealTime::PlainToText(FBTextDisplay display, double plain) const
{
  int discrete = static_cast<int>(std::round(plain));
  return NoteNames[discrete % 12] + std::to_string(discrete / 12 - 1);
}

std::optional<double>
FBNoteParamNonRealTime::TextToPlain(FBTextDisplay display, std::string const& text) const
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
        noteMenu.addItem(midiNote + 1, PlainToText(FBTextDisplay::Text, midiNote));
    }
    result.addSubMenu(NoteNames[i], noteMenu);
  }
  return result;
}