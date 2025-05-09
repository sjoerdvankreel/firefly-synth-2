#pragma once

#include <playground_base/dsp/host/FBHostBlock.hpp>

enum class FBVoiceState { Free, Active, Returned };

struct FBVoiceInfo final
{
  int slot = {};
  int initialOffset = {};
  FBNoteEvent event = {};
  FBVoiceState state = {};
};
