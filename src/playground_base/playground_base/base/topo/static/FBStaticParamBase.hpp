#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <playground_base/base/topo/param/FBLog2Param.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBBarsParam.hpp>
#include <playground_base/base/topo/param/FBTextDisplay.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>

#include <playground_base/base/topo/static/FBParamType.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParamBase
{
private:
  // TODO
  FBLog2Param log2 = {};
  FBListParam list = {};
  FBNoteParam note = {};
  FBBarsParam bars = {};
  FBBoolParamNonRealTime boolean = {};
  FBLinearParam linear = {};
  FBDiscreteParamNonRealTime discrete = {};
  FBDiscreteLog2Param discreteLog2 = {};

public:
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string tooltip = {};
  std::string defaultText = {};
  FBParamType type = (FBParamType)-1;
  FBParamsDependencies dependencies = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParamBase);

  int ValueCount() const;

  FBListParam& List();
  FBNoteParam& Note();
  FBBarsParam& Bars();
  FBLog2Param& Log2();
  FBBoolParam& Boolean();
  FBLinearParam& Linear();
  FBDiscreteParam& Discrete();
  FBDiscreteLog2Param& DiscreteLog2();
  
  FBListParam const& List() const;
  FBNoteParam const& Note() const;
  FBBarsParam const& Bars() const;
  FBLog2Param const& Log2() const;
  FBBoolParam const& Boolean() const;
  FBLinearParam const& Linear() const;
  FBDiscreteParam const& Discrete() const;
  FBDiscreteLog2Param const& DiscreteLog2() const;
  
  FBParamNonRealTime const& NonRealTime() const;

  // TODO check all this
  // TODO also check all invocations of the nonrealtime stuff
  // TODO also make sure thats all converted to double
  // TODO also make sure no cstyle casts
  // TODO finally get rid of all the switches
  juce::PopupMenu MakePopupMenu() const;
  float DefaultNormalizedByText() const;
  float AnyDiscreteToNormalizedSlow(int discrete) const;
  int NormalizedToAnyDiscreteSlow(float normalized) const;

  std::string NormalizedToText(FBValueTextDisplay display, float normalized) const;
  std::string NormalizedToText(FBParamTextDisplay display, float normalized) const;
  std::optional<float> TextToNormalized(FBValueTextDisplay display, std::string const& text) const;
};

inline FBListParam&
FBStaticParamBase::List()
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam&
FBStaticParamBase::Note()
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBarsParam&
FBStaticParamBase::Bars()
{
  assert(type == FBParamType::Bars);
  return bars;
}

inline FBBoolParam&
FBStaticParamBase::Boolean()
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam&
FBStaticParamBase::Linear()
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBLog2Param&
FBStaticParamBase::Log2()
{
  assert(type == FBParamType::Log2);
  return log2;
}

inline FBDiscreteParam&
FBStaticParamBase::Discrete()
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBDiscreteLog2Param&
FBStaticParamBase::DiscreteLog2()
{
  assert(type == FBParamType::DiscreteLog2);
  return discreteLog2;
}

inline FBListParam const&
FBStaticParamBase::List() const
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam const&
FBStaticParamBase::Note() const
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBarsParam const&
FBStaticParamBase::Bars() const
{
  assert(type == FBParamType::Bars);
  return bars;
}

inline FBBoolParam const&
FBStaticParamBase::Boolean() const
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam const&
FBStaticParamBase::Linear() const
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBLog2Param const&
FBStaticParamBase::Log2() const
{
  assert(type == FBParamType::Log2);
  return log2;
}

inline FBDiscreteParam const&
FBStaticParamBase::Discrete() const
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBDiscreteLog2Param const&
FBStaticParamBase::DiscreteLog2() const
{
  assert(type == FBParamType::DiscreteLog2);
  return discreteLog2;
}

inline FBParamNonRealTime const& 
FBStaticParamBase::NonRealTime() const
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline int
FBStaticParamBase::ValueCount() const
{
  switch (type)
  {
  case FBParamType::List: return list.ValueCount();
  case FBParamType::Note: return note.ValueCount();
  case FBParamType::Bars: return bars.ValueCount();
  case FBParamType::Log2: return log2.ValueCount();
  case FBParamType::Linear: return linear.ValueCount();
  case FBParamType::Boolean: return boolean.ValueCount();
  case FBParamType::Discrete: return discrete.ValueCount();
  case FBParamType::DiscreteLog2: return discreteLog2.ValueCount();
  default: assert(false); return {};
  }
}