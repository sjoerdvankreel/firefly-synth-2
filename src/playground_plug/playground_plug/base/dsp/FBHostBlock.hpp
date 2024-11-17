#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <vector>

// TODO nondestructive + per-voice
struct FBAutoEvent
{
  int tag;
  int position;
  float normalized;
};

struct FBNoteEvent
{
  int id;
  bool on;
  float velo;
  short key;
  short channel;
};

struct FBHostBlock
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FBHostBlock);

  int sampleCount;
  float* const* audioOut;
  float const* const* audioIn;
  std::vector<FBNoteEvent> noteEvents = { 1024, FBNoteEvent() };
  std::vector<FBAutoEvent> autoEvents = { 1024, FBAutoEvent() };
};