#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/topo/static/FBScalarParamAddrs.hpp>

#include <array>
#include <functional>

class FBVoiceAccParamState;
class FBGlobalAccParamState;
class FBVoiceBlockParamState;
class FBGlobalBlockParamState;

typedef std::function<FBVoiceAccParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBVoiceAccProcParamAddrSelector;
typedef std::function<FBGlobalAccParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBGlobalAccProcParamAddrSelector;
typedef std::function<FBVoiceBlockParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBVoiceBlockProcParamAddrSelector;
typedef std::function<FBGlobalBlockParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBGlobalBlockProcParamAddrSelector;

typedef std::function<float* (
  int moduleSlot, int paramSlot, void* state)>
FBGlobalExchangeParamAddrSelector;
typedef std::function<std::array<float, FBMaxVoices>* (
  int moduleSlot, int paramSlot, void* state)>
FBVoiceExchangeParamAddrSelector;

struct FBParamAddrSelectors final
{
  FB_COPY_MOVE_DEFCTOR(FBParamAddrSelectors);
  FBScalarParamAddrSelector scalar = {};
  FBVoiceExchangeParamAddrSelector voiceExchange = {};
  FBGlobalExchangeParamAddrSelector globalExchange = {};
  FBVoiceAccProcParamAddrSelector voiceAccProc = {};
  FBGlobalAccProcParamAddrSelector globalAccProc = {};
  FBVoiceBlockProcParamAddrSelector voiceBlockProc = {};
  FBGlobalBlockProcParamAddrSelector globalBlockProc = {};
};