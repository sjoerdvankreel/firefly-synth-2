#pragma once

struct FBNoteId
{
  int id;
  int key;
  int channel;
};

struct FBNoteEvent
{
  int pos;
  bool on;
  float velo;
  FBNoteId note;
};