#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <vector>

enum FBNoteEventType
{
  FBNoteEventTypeOn,
  FBNoteEventTypeOff,
  FBNoteEventTypeCut
};

// TODO nondestructive + per-voice
struct FBAutoEvent
{
  int paramTag;
  int position;
  float normalized;
};

struct FBNoteEvent
{
  short key;
  short port;
  short channel;

  int id;
  float velo;
  FBNoteEventType type;
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