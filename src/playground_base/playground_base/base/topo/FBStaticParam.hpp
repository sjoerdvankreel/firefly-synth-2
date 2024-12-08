#pragma once

#include <playground_base/base/topo/FBListItem.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <string>
#include <vector>
#include <optional>
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
  double defaultNormalized = 0.0;
  std::vector<FBListItem> list = {};

  FBScalarAddrSelector scalarAddr = {};
  FBVoiceAccAddrSelector voiceAccAddr = {};
  FBGlobalAccAddrSelector globalAccAddr = {};
  FBVoiceBlockAddrSelector voiceBlockAddr = {};
  FBGlobalBlockAddrSelector globalBlockAddr = {};

  double DiscreteToNormalized(int index) const
  { return index / (valueCount - 1.0); }
  bool NormalizedToBool(double normalized) const
  { return NormalizedToDiscrete(normalized) != 0; }
  int NormalizedToDiscrete(double normalized) const
  { return std::min(valueCount - 1, (int)(normalized * valueCount)); }

  double TextToNormalizedOrDefault(std::string const& text, bool io) const
  { return TextToNormalized(text, io).value_or(defaultNormalized); }

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
  std::string NormalizedToText(bool io, double normalized) const;
  std::optional<double> TextToNormalized(std::string const& text, bool io) const;
};