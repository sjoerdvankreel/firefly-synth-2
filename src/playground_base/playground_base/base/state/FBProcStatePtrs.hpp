#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <vector>

struct FBScalarStatePtrs;
struct FBProcVoiceAccParamState;
struct FBProcGlobalAccParamState;

struct FBProcStatePtrs final
{
  FB_NOCOPY_MOVE_DEFCTOR(FBProcStatePtrs);
  void InitFrom(FBScalarStatePtrs const& scalar);

  std::vector<bool> isAcc = {};
  std::vector<bool> isVoice = {};
  std::vector<float*> block = {};
  std::vector<FBProcVoiceAccParamState*> voiceAcc = {};
  std::vector<FBProcGlobalAccParamState*> globalAcc = {};
};