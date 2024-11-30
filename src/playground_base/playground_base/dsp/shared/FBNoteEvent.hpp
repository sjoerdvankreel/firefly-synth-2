#pragma once

struct FBNoteId
{
  int id;
  int key;
  int channel;
};

struct FBNoteEvent
{
  bool on;
  float velo;
  int position;
  FBNoteId note;
};