#include <playground_base/gui/shared/FBGUIGraphing.hpp>

FBNoteEvent
FBDetailMakeNoteC4Off(int pos)
{
  FBNoteEvent result;
  result.pos = pos;
  result.on = false;
  result.velo = 0.0f;
  result.note.id = 0;
  result.note.key = 60;
  result.note.channel = 0;
  return result;
}