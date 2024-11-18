#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
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

// host throws 1 of these at us on each round,
// but internally we use fixed block size
struct FBHostBlock
{
  FB_NOCOPY_NOMOVE(FBHostBlock);
  FBHostBlock(int maxSampleCount);

  int currentSampleCount;

  // these could be raw pointers avoiding a copy
  // but we re-use the FBHostBlock struct for block splitting
  // probably 1 in/out copy per block wont kill us
  std::array<std::vector<float>, FB_CHANNELS_STEREO> audioIn;
  std::array<std::vector<float>, FB_CHANNELS_STEREO> audioOut;

  // note and auto must be sorted by sample position
  std::vector<FBNoteEvent> noteEvents = { 1024, FBNoteEvent() };
  std::vector<FBAutoEvent> autoEvents = { 1024, FBAutoEvent() };
  std::vector<FBPlugEvent> plugEvents = { 1024, FBPlugEvent() };
};