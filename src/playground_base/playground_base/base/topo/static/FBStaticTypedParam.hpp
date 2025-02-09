#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBParamType.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBFreqOctParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

#include <cassert>

struct FBStaticTypedParam final
{
private:
  FBListParam list = {};
  FBNoteParam note = {};
  FBBoolParam boolean = {};
  FBLinearParam linear = {};
  FBFreqOctParam freqOct = {};
  FBDiscreteParam discrete = {};

public:
  FBParamType type = (FBParamType)-1;
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTypedParam);

  FBListParam& List();
  FBNoteParam& Note();
  FBBoolParam& Boolean();
  FBLinearParam& Linear();
  FBFreqOctParam& FreqOct();
  FBDiscreteParam& Discrete();

  FBListParam const& List() const;
  FBNoteParam const& Note() const;
  FBBoolParam const& Boolean() const;
  FBLinearParam const& Linear() const;
  FBFreqOctParam const& FreqOct() const;
  FBDiscreteParam const& Discrete() const;
};

inline FBListParam&
FBStaticTypedParam::List()
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam&
FBStaticTypedParam::Note()
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBoolParam&
FBStaticTypedParam::Boolean()
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam&
FBStaticTypedParam::Linear()
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBFreqOctParam&
FBStaticTypedParam::FreqOct()
{
  assert(type == FBParamType::FreqOct);
  return freqOct;
}

inline FBDiscreteParam& 
FBStaticTypedParam::Discrete()
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBListParam const&
FBStaticTypedParam::List() const
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam const&
FBStaticTypedParam::Note() const
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBoolParam const&
FBStaticTypedParam::Boolean() const
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam const&
FBStaticTypedParam::Linear() const
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBFreqOctParam const&
FBStaticTypedParam::FreqOct() const
{
  assert(type == FBParamType::FreqOct);
  return freqOct;
}

inline FBDiscreteParam const&
FBStaticTypedParam::Discrete() const
{
  assert(type == FBParamType::Discrete);
  return discrete;
}