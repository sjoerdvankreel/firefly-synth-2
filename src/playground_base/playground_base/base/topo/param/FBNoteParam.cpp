#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <vector>

using namespace juce;

static const std::vector<std::string> NoteNames = 
{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

float 
FBNoteParam::PlainToNormalized(int plain) const
{
  return std::clamp(plain / (ValueCount() - 1.0f), 0.0f, 1.0f);
}

std::string 
FBNoteParam::PlainToText(int plain) const
{
  return NoteNames[plain % 12] + std::to_string(plain / 12 - 1);
}

std::optional<int>
FBNoteParam::TextToPlain(std::string const& text) const
{
  for (int i = 0; i < ValueCount(); i++)
    if (text == PlainToText(i))
      return { i };
  return {};
}

PopupMenu
FBNoteParam::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < NoteNames.size(); i++)
  {
    PopupMenu noteMenu;
    for (int j = 0; j < 128.0f / 12.0f; j++)
    {
      int midiNote = j * 12 + i;
      if (midiNote < 128)
        noteMenu.addItem(midiNote + 1, PlainToText(midiNote));
    }
    result.addSubMenu(NoteNames[i], noteMenu);
  }
  return result;
}