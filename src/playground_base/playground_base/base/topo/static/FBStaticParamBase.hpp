#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

#include <playground_base/base/topo/param/FBLog2Param.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBBarsParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBIdentityParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>
#include <functional>

typedef std::function<std::string(int slot)>
FBParamSlotFormatter;

struct FBStaticParamBase
{
private:
  FBLog2ParamNonRealTime log2 = {};
  FBListParamNonRealTime list = {};
  FBBarsParamNonRealTime bars = {};
  FBBoolParamNonRealTime boolean = {};
  FBLinearParamNonRealTime linear = {};
  FBDiscreteParamNonRealTime discrete = {};
  FBIdentityParamNonRealTime identity = {};

public:
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string display = {};
  std::string defaultText = {};
  FBParamType type = (FBParamType)-1;
  FBParamsDependencies dependencies = {};
  FBParamSlotFormatter slotFormatter = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParamBase);

  FBListParam& List();
  FBBarsParam& Bars();
  FBLog2Param& Log2();
  FBBoolParam& Boolean();
  FBLinearParam& Linear();
  FBDiscreteParam& Discrete();
  FBIdentityParam& Identity();
  
  FBListParam const& List() const;
  FBBarsParam const& Bars() const;
  FBLog2Param const& Log2() const;
  FBBoolParam const& Boolean() const;
  FBLinearParam const& Linear() const;
  FBDiscreteParam const& Discrete() const;
  FBIdentityParam const& Identity() const;
  
  FBParamNonRealTime const& NonRealTime() const;
  FBItemsParamNonRealTime const& ItemsNonRealTime() const;

  double DefaultNormalizedByText() const;
  std::string NormalizedToTextWithUnit(bool io, double normalized) const;
};

inline FBListParam&
FBStaticParamBase::List()
{
  assert(type == FBParamType::List);
  return list;
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

inline FBIdentityParam&
FBStaticParamBase::Identity()
{
  assert(type == FBParamType::Identity);
  return identity;
}

inline FBListParam const&
FBStaticParamBase::List() const
{
  assert(type == FBParamType::List);
  return list;
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

inline FBIdentityParam const&
FBStaticParamBase::Identity() const
{
  assert(type == FBParamType::Identity);
  return identity;
}