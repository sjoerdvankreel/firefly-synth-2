#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <vector>

enum FBNoteEventType
{
  FBNoteEventTypeOn,
  FBNoteEventTypeOff,
  FBNoteEventTypeCut
};

struct FBAutoEvent
{
  int voice;
  int paramTag;
  int position;
  float normalized;
  bool destructive;
};

struct FBNoteEvent
{
  float velo;
  short key;
  short port;
  short channel;
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