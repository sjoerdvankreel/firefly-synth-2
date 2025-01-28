#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

#include <unordered_map>

class FBPlugGUI;
struct FBRuntimeTopo;

struct FBGraphProcState final
{
  FBProcStateContainer container;
  FBVoiceManager voiceManager;
  FBFixedFloatAudioBlock audio;
  std::vector<FBNoteEvent> notes;
  FBPlugInputBlock input;

  FBGraphProcState(FBPlugGUI const* plugGUI);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGraphProcState);
};