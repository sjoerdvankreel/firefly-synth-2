#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBBarsParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBFreqOctParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

#include <playground_base/base/topo/static/FBProcParamAddrs.hpp>
#include <playground_base/base/topo/static/FBAutomationType.hpp>
#include <playground_base/base/topo/static/FBStaticParamBase.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParam final:
public FBStaticParamBase
{
public:
  bool acc = false;
  bool output = false;
  FBParamsDependencies dependencies = {};
  FBParamAddrSelectors addrSelectors = {};

  bool IsAcc() const;
  bool IsVoice() const;
  bool IsVoiceAcc() const;
  bool IsGlobalAcc() const;
  bool IsVoiceBlock() const;
  bool IsGlobalBlock() const;

  std::string AutomationTooltip() const;
  FBAutomationType AutomationType() const;
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
};

inline bool
FBStaticParam::IsVoiceAcc() const
{
  return addrSelectors.voiceAccProc != nullptr;
}

inline bool
FBStaticParam::IsGlobalAcc() const
{
  return addrSelectors.globalAccProc != nullptr;
}

inline bool
FBStaticParam::IsVoiceBlock() const
{
  return addrSelectors.voiceBlockProc != nullptr;
}

inline bool
FBStaticParam::IsGlobalBlock() const
{
  return addrSelectors.globalBlockProc != nullptr;
}

inline bool
FBStaticParam::IsAcc() const
{
  return IsVoiceAcc() || IsGlobalAcc();
}

inline bool
FBStaticParam::IsVoice() const
{
  return IsVoiceAcc() || IsVoiceBlock();
}