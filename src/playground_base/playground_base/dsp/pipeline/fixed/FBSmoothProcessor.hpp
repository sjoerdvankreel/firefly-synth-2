#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccModEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccAutoEvent.hpp>

#include <set>
#include <array>
#include <vector>

class FBVoiceManager;
struct FBFixedInputBlock;
struct FBFixedOutputBlock;

class FBSmoothProcessor final
{
  FBVoiceManager* const _voiceManager ;
  std::set<int> _activeGlobalSmoothing = {};
  std::set<int> _finishedGlobalSmoothing = {};
  std::vector<FBAccAutoEvent> _accAutoBySampleThenParam = {};
  std::vector<FBAccModEvent> _accModBySampleThenParamThenNote = {};
  std::array<std::set<int>, FBMaxVoices> _activeVoiceSmoothing = {};
  std::array<std::set<int>, FBMaxVoices> _finishedVoiceSmoothing = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBSmoothProcessor);
  FBSmoothProcessor(FBVoiceManager* voiceManager);
  void ProcessSmoothing(FBFixedInputBlock const& input, FBFixedOutputBlock& output);
};