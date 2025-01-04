#pragma once

#include <functional>

class FBVoiceAccParamState;
class FBGlobalAccParamState;
class FBVoiceBlockParamState;
class FBGlobalBlockParamState;

typedef std::function<float* (
  int moduleSlot, int paramSlot, void* state)>
FBScalarAddrSelector;
typedef std::function<FBVoiceAccParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBVoiceAccAddrSelector;
typedef std::function<FBGlobalAccParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBGlobalAccAddrSelector;
typedef std::function<FBVoiceBlockParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBVoiceBlockAddrSelector;
typedef std::function<FBGlobalBlockParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBGlobalBlockAddrSelector;