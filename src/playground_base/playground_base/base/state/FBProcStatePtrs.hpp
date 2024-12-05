#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

#include <array>
#include <vector>

struct FBProcParamState;
struct FBScalarStatePtrs;

struct FBProcSingleStatePtrs final
{
  std::vector<float*> block = {};
  std::vector<FBProcParamState*> acc = {};
  
  FB_NOCOPY_MOVE_DEFCTOR(FBProcSingleStatePtrs);
  void InitFrom(
    bool voice,
    std::vector<bool> const& isAcc,
    std::vector<bool> const& isVoice,
    FBScalarStatePtrs const& single);
};

struct FBProcStatePtrs final
{
  FB_NOCOPY_MOVE_DEFCTOR(FBProcStatePtrs);
  void InitFrom(FBScalarStatePtrs const& scalar);

  std::vector<bool> isAcc = {};
  std::vector<bool> isVoice = {};
  FBProcSingleStatePtrs single = {};
  std::array<FBProcSingleStatePtrs, FB_MAX_VOICES> voice = {};
};