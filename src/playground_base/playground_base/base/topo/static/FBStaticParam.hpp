#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <playground_base/base/topo/static/FBParamAddrs.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
#include <playground_base/base/topo/static/FBAutomationType.hpp>
#include <playground_base/base/topo/static/FBParamsDependency.hpp>
#include <playground_base/base/topo/static/FBStaticTypedParam.hpp>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParam final
{
  bool acc = false;
  bool output = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string tooltip = {};
  std::string defaultText = {};
  FBStaticTypedParam typed = {};
  FBParamsDependency relevant = {};
  FBParamAddrSelectors addrSelectors = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);

  bool IsAcc() const;
  bool IsVoice() const;
  bool IsVoiceAcc() const;
  bool IsGlobalAcc() const;
  bool IsVoiceBlock() const;
  bool IsGlobalBlock() const;
  
  int ValueCount() const;
  std::string AutomationTooltip() const;
  FBAutomationType AutomationType() const;

  float AnyDiscreteToNormalizedSlow(int discrete) const;
  int NormalizedToAnyDiscreteSlow(float normalized) const;

  float DefaultNormalizedByText() const;
  std::optional<float> TextToNormalized(bool io, std::string const& text) const;
  std::string NormalizedToText(FBValueTextDisplay display, float normalized) const;
  std::string NormalizedToText(FBParamTextDisplay display, float normalized) const;
};

inline int
FBStaticParam::ValueCount() const
{
  switch (typed.type)
  {
  case FBParamType::List: return typed.List().ValueCount();
  case FBParamType::Note: return typed.Note().ValueCount();
  case FBParamType::Linear: return typed.Linear().ValueCount();
  case FBParamType::FreqOct: return typed.FreqOct().ValueCount();
  case FBParamType::Boolean: return typed.Boolean().ValueCount();
  case FBParamType::Discrete: return typed.Discrete().ValueCount();
  default: assert(false); return {};
  }
}