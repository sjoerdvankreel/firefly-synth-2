#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <playground_base/base/topo/param/FBLog2Param.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBBarsParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>

#include <playground_base/base/topo/static/FBParamType.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
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
  FBLog2ParamNonRealTime log2 = {};
  FBListParamNonRealTime list = {};
  FBNoteParamNonRealTime note = {};
  FBBarsParamNonRealTime bars = {};
  FBBoolParamNonRealTime boolean = {};
  FBLinearParamNonRealTime linear = {};
  FBDiscreteParamNonRealTime discrete = {};
  FBDiscreteLog2ParamNonRealTime discreteLog2 = {};

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

  FBListParamRealTime& ListRealTime();
  FBNoteParamRealTime& NoteRealTime();
  FBBarsParamRealTime& BarsRealTime();
  FBLog2ParamRealTime& Log2RealTime();
  FBBoolParamRealTime& BooleanRealTime();
  FBLinearParamRealTime& LinearRealTime();
  FBDiscreteParamRealTime& DiscreteRealTime();
  FBDiscreteLog2ParamRealTime& DiscreteLog2RealTime();

  FBListParamRealTime const& ListRealTime() const;
  FBNoteParamRealTime const& NoteRealTime() const;
  FBBarsParamRealTime const& BarsRealTime() const;
  FBLog2ParamRealTime const& Log2RealTime() const;
  FBBoolParamRealTime const& BooleanRealTime() const;
  FBLinearParamRealTime const& LinearRealTime() const;
  FBDiscreteParamRealTime const& DiscreteRealTime() const;
  FBDiscreteLog2ParamRealTime const& DiscreteLog2RealTime() const;

  IFBParamNonRealTime& NonRealTime();
  FBItemsParamNonRealTime& ItemsNonRealTime();
  FBSteppedParamNonRealTime& SteppedNonRealTime();
  FBContinuousParamNonRealTime& ContinuousNonRealTime();

  IFBParamNonRealTime const& NonRealTime() const;
  FBItemsParamNonRealTime const& ItemsNonRealTime() const;
  FBSteppedParamNonRealTime const& SteppedNonRealTime() const;
  FBContinuousParamNonRealTime const& ContinuousNonRealTime() const;

  float DefaultNormalizedByText() const;
  std::string NormalizedToText(FBParamTextDisplay display, float normalized) const;
  std::optional<float> TextToNormalized(FBValueTextDisplay display, std::string const& text) const;
};

inline FBListParamRealTime&
FBStaticParamBase::ListRealTime()
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParamRealTime&
FBStaticParamBase::NoteRealTime()
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBarsParamRealTime&
FBStaticParamBase::BarsRealTime()
{
  assert(type == FBParamType::Bars);
  return bars;
}

inline FBBoolParamRealTime&
FBStaticParamBase::BooleanRealTime()
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParamRealTime&
FBStaticParamBase::LinearRealTime()
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBLog2ParamRealTime&
FBStaticParamBase::Log2RealTime()
{
  assert(type == FBParamType::Log2);
  return log2;
}

inline FBDiscreteParamRealTime&
FBStaticParamBase::DiscreteRealTime()
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBDiscreteLog2ParamRealTime&
FBStaticParamBase::DiscreteLog2RealTime()
{
  assert(type == FBParamType::DiscreteLog2);
  return discreteLog2;
}

inline FBListParamRealTime const&
FBStaticParamBase::ListRealTime() const
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParamRealTime const&
FBStaticParamBase::NoteRealTime() const
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBarsParamRealTime const&
FBStaticParamBase::BarsRealTime() const
{
  assert(type == FBParamType::Bars);
  return bars;
}

inline FBBoolParamRealTime const&
FBStaticParamBase::BooleanRealTime() const
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParamRealTime const&
FBStaticParamBase::LinearRealTime() const
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBLog2ParamRealTime const&
FBStaticParamBase::Log2RealTime() const
{
  assert(type == FBParamType::Log2);
  return log2;
}

inline FBDiscreteParamRealTime const&
FBStaticParamBase::DiscreteRealTime() const
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBDiscreteLog2ParamRealTime const&
FBStaticParamBase::DiscreteLog2RealTime() const
{
  assert(type == FBParamType::DiscreteLog2);
  return discreteLog2;
}