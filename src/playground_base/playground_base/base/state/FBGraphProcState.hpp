#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>

#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

class FBPlugGUI;

struct FBGraphProcState final
{
  FBProcStateContainer container;
  FBVoiceManager voiceManager;
  FBFixedFloatAudioBlock audio;
  std::vector<FBNoteEvent> notes;
  FBPlugInputBlock input;
  FBModuleProcState moduleState;

  FBGraphProcState(FBPlugGUI const* plugGUI);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGraphProcState);
};