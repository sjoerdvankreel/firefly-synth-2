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

PopupMenu
FBNoteParam::MakePopupMenu() const
{
  PopupMenu result;
  int notesPerOct = (int)NoteNames.size();
  for (int i = 0; i < notesPerOct; i++)
  {
    PopupMenu noteMenu;
    for (int j = 0; j < ValueCount(); j++)
      if (j % notesPerOct == i)
        noteMenu.addItem(j + 1, PlainToText(j));
    result.addSubMenu(NoteNames[i], noteMenu);
  }
  return result;
}

std::optional<int>
FBNoteParam::TextToPlain(std::string const& text) const
{
  for (int i = 0; i < ValueCount(); i++)
    if (text == PlainToText(i))
      return { i };
  return {};
}