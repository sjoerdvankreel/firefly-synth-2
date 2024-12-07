#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>

#include <array>
#include <vector>

struct FBScalarStatePtrs;
struct FBVoiceAccParamState;
struct FBGlobalAccParamState;

struct FBProcStatePtrs final
{
  FB_NOCOPY_MOVE_DEFCTOR(FBProcStatePtrs);
  void InitFrom(FBScalarStatePtrs const& scalar);

  std::vector<bool> isAcc = {};
  std::vector<bool> isVoice = {};
  std::vector<float*> allBlock = {};
  std::vector<FBVoiceAccParamState*> voiceAcc = {};
  std::vector<FBGlobalAccParamState*> globalAcc = {};
  std::vector<FBVoiceBlockParamState> voiceBlock = {};
};