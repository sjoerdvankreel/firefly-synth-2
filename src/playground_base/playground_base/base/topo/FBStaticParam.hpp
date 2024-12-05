#pragma once

#include <playground_base/base/topo/FBListItem.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <string>
#include <vector>
#include <optional>
#include <functional>

struct FBProcParamState;

typedef std::function<float* (
  int moduleSlot, int paramSlot, void* state)>
FBSingleFloatAddrSelector;
typedef std::function<float* (
  int voice, int moduleSlot, int paramSlot, void* state)>
FBVoiceFloatAddrSelector;

typedef std::function<FBProcParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBSingleProcParamAddrSelector;
typedef std::function<FBProcParamState* (
  int voice, int moduleSlot, int paramSlot, void* state)>
FBVoiceProcParamAddrSelector;

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

  FBSingleFloatAddrSelector scalarAddr = {};
  FBVoiceFloatAddrSelector procVoiceBlockAddr = {};
  FBSingleFloatAddrSelector procSingleBlockAddr = {};
  FBVoiceProcParamAddrSelector procVoiceAccAddr = {};
  FBSingleProcParamAddrSelector procSingleAccAddr = {};

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