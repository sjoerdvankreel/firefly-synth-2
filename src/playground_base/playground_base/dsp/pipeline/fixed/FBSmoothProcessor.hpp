#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccEvent.hpp>

#include <vector>

class FBVoiceManager;
struct FBFixedInputBlock;
struct FBFixedOutputBlock;

class FBSmoothProcessor final
{
  FBVoiceManager* const _voiceManager ;
  std::vector<FBAccEvent> _accBySampleThenParam = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBSmoothProcessor);
  FBSmoothProcessor(FBVoiceManager* voiceManager);
  void ProcessSmoothing(FBFixedInputBlock const& input, FBFixedOutputBlock& output);
};