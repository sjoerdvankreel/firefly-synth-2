#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
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

struct FBStaticAudioParam final:
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
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticAudioParam);
};

inline bool
FBStaticAudioParam::IsVoiceAcc() const
{
  return addrSelectors.voiceAccProc != nullptr;
}

inline bool
FBStaticAudioParam::IsGlobalAcc() const
{
  return addrSelectors.globalAccProc != nullptr;
}

inline bool
FBStaticAudioParam::IsVoiceBlock() const
{
  return addrSelectors.voiceBlockProc != nullptr;
}

inline bool
FBStaticAudioParam::IsGlobalBlock() const
{
  return addrSelectors.globalBlockProc != nullptr;
}

inline bool
FBStaticAudioParam::IsAcc() const
{
  return IsVoiceAcc() || IsGlobalAcc();
}

inline bool
FBStaticAudioParam::IsVoice() const
{
  return IsVoiceAcc() || IsVoiceBlock();
}