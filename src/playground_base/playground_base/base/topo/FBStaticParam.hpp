#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBTextDisplay.hpp>
#include <playground_base/base/topo/FBAutomationType.hpp>
#include <playground_base/base/topo/FBParamsDependency.hpp>

#include <playground_base/base/topo/param/FBParamType.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBFreqOctParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>
#include <playground_base/base/topo/param/FBParamAddrSelector.hpp>

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

  FBParamType type = {};
  FBListParam list = {};
  FBBoolParam boolean = {};
  FBLinearParam linear = {};
  FBFreqOctParam freqOct = {};
  FBDiscreteParam discrete = {};
  FBParamsDependency relevant = {};

  FBScalarAddrSelector scalarAddr = {};
  FBVoiceAccAddrSelector voiceAccAddr = {};
  FBGlobalAccAddrSelector globalAccAddr = {};
  FBVoiceBlockAddrSelector voiceBlockAddr = {};
  FBGlobalBlockAddrSelector globalBlockAddr = {};    
  FBVoiceExchangeAddrSelector voiceExchangeAddr = {};
  FBGlobalExchangeAddrSelector globalExchangeAddr = {};

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
  std::string NormalizedToText(FBTextDisplay display, float normalized) const;
  std::optional<float> TextToNormalized(bool io, std::string const& text) const;
};

inline int
FBStaticParam::ValueCount() const
{
  switch (type)
  {
  case FBParamType::List: return list.ValueCount();
  case FBParamType::Linear: return linear.ValueCount();
  case FBParamType::FreqOct: return freqOct.ValueCount();
  case FBParamType::Boolean: return boolean.ValueCount();
  case FBParamType::Discrete: return discrete.ValueCount();
  default: assert(false); return {};
  }
}