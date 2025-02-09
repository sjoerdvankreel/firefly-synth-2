#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <vector>

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
  // TODO wtf -1?
  return NoteNames[plain % 12] + std::to_string(plain / 12 - 1);
}

std::optional<int> 
FBNoteParam::TextToPlain(bool io, std::string const& text) const
{
  for (int i = 0; i < ValueCount(); i++)
    if (text == PlainToText(i))
      return { i };
  return {};
}