#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <vector>

struct FBPlugEvent
{
  int tag;
  float normalized;
};

// TODO nondestructive + per-voice
struct FBAutoEvent
{
  int tag;
  int position;
  float normalized;
};

struct FBNoteEvent
{
  bool on;
  float velo;
  
  int id;
  int key;
  int channel;
  int position;
};

struct FBHostBlock
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FBHostBlock);

  int sampleCount;
  float* const* audioOut;
  float const* const* audioIn;

  // note and auto must be sorted by sample position
  std::vector<FBNoteEvent> noteEvents = { 1024, FBNoteEvent() };
  std::vector<FBAutoEvent> autoEvents = { 1024, FBAutoEvent() };
  std::vector<FBPlugEvent> plugEvents = { 1024, FBPlugEvent() };
};