#pragma once

#include <playground_base/base/topo/FBListItem.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>
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

struct FBStaticParam final
{
  bool acc = false;
  int slotCount = {};
  int valueCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  float plainMin = 0.0f;
  float plainMax = 1.0f; // TODO logmidpoint and pct
  float defaultNormalized = 0.0f;
  bool percentage = false;
  std::vector<FBListItem> list = {};

  FBScalarAddrSelector scalarAddr = {};
  FBVoiceAccAddrSelector voiceAccAddr = {};
  FBGlobalAccAddrSelector globalAccAddr = {};
  FBVoiceBlockAddrSelector voiceBlockAddr = {};
  FBGlobalBlockAddrSelector globalBlockAddr = {};

  bool NormalizedToBool(float normalized) const
  { return NormalizedToDiscrete(normalized) != 0; }
  float TextToNormalizedOrDefault(std::string const& text, bool io) const
  { return TextToNormalized(text, io).value_or(defaultNormalized); }

  float DiscreteToNormalized(int discrete) const
  { return std::clamp(discrete / (valueCount - 1.0f), 0.0f, 1.0f); }
  int NormalizedToDiscrete(float normalized) const
  { return std::clamp((int)(normalized * valueCount), 0, valueCount - 1); }

  float NormalizedToPlainLinear(float normalized) const
  { return plainMin + (plainMax - plainMin) * normalized; }
  float PlainLinearToNormalized(float plain) const
  { return std::clamp((plain - plainMin) / (plainMax - plainMin), 0.0f, 1.0f); }

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
  std::string NormalizedToText(bool io, float normalized) const;
  std::optional<float> TextToNormalized(std::string const& text, bool io) const;
};