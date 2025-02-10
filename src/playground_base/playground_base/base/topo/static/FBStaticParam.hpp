#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBTimeSigParam.hpp>
#include <playground_base/base/topo/param/FBFreqOctParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

#include <playground_base/base/topo/static/FBParamType.hpp>
#include <playground_base/base/topo/static/FBParamAddrs.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
#include <playground_base/base/topo/static/FBAutomationType.hpp>
#include <playground_base/base/topo/static/FBParamsDependency.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParam final
{
private:
  FBListParam list = {};
  FBNoteParam note = {};
  FBBoolParam boolean = {};
  FBLinearParam linear = {};
  FBFreqOctParam freqOct = {};
  FBTimeSigParam timeSig = {};
  FBDiscreteParam discrete = {};

public:
  bool acc = false;
  bool output = false;
  int slotCount = {};
  FBParamType type = (FBParamType)-1;

  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string tooltip = {};
  std::string defaultText = {};
  FBParamsDependency relevant = {};
  FBParamAddrSelectors addrSelectors = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);

  bool IsAcc() const;
  bool IsVoice() const;
  bool IsVoiceAcc() const;
  bool IsGlobalAcc() const;
  bool IsVoiceBlock() const;
  bool IsGlobalBlock() const;

  FBListParam& List();
  FBNoteParam& Note();
  FBBoolParam& Boolean();
  FBLinearParam& Linear();
  FBFreqOctParam& FreqOct();
  FBTimeSigParam& TimeSig();
  FBDiscreteParam& Discrete();

  FBListParam const& List() const;
  FBNoteParam const& Note() const;
  FBBoolParam const& Boolean() const;
  FBLinearParam const& Linear() const;
  FBFreqOctParam const& FreqOct() const;
  FBTimeSigParam const& TimeSig() const;
  FBDiscreteParam const& Discrete() const;
  
  int ValueCount() const;
  std::string AutomationTooltip() const;
  FBAutomationType AutomationType() const;

  float AnyDiscreteToNormalizedSlow(int discrete) const;
  int NormalizedToAnyDiscreteSlow(float normalized) const;

  juce::PopupMenu MakePopupMenu() const;
  float DefaultNormalizedByText() const;
  std::optional<float> TextToNormalized(bool io, std::string const& text) const;
  std::string NormalizedToText(FBValueTextDisplay display, float normalized) const;
  std::string NormalizedToText(FBParamTextDisplay display, float normalized) const;
};

inline FBListParam&
FBStaticParam::List()
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam&
FBStaticParam::Note()
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBTimeSigParam&
FBStaticParam::TimeSig()
{
  assert(type == FBParamType::TimeSig);
  return timeSig;
}

inline FBBoolParam&
FBStaticParam::Boolean()
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam&
FBStaticParam::Linear()
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBFreqOctParam&
FBStaticParam::FreqOct()
{
  assert(type == FBParamType::FreqOct);
  return freqOct;
}

inline FBDiscreteParam&
FBStaticParam::Discrete()
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBListParam const&
FBStaticParam::List() const
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam const&
FBStaticParam::Note() const
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBTimeSigParam const&
FBStaticParam::TimeSig() const
{
  assert(type == FBParamType::TimeSig);
  return timeSig;
}

inline FBBoolParam const&
FBStaticParam::Boolean() const
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam const&
FBStaticParam::Linear() const
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBFreqOctParam const&
FBStaticParam::FreqOct() const
{
  assert(type == FBParamType::FreqOct);
  return freqOct;
}

inline FBDiscreteParam const&
FBStaticParam::Discrete() const
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline int
FBStaticParam::ValueCount() const
{
  switch (type)
  {
  case FBParamType::List: return list.ValueCount();
  case FBParamType::Note: return note.ValueCount();
  case FBParamType::Linear: return linear.ValueCount();
  case FBParamType::FreqOct: return freqOct.ValueCount();
  case FBParamType::TimeSig: return timeSig.ValueCount();
  case FBParamType::Boolean: return boolean.ValueCount();
  case FBParamType::Discrete: return discrete.ValueCount();
  default: assert(false); return {};
  }
}