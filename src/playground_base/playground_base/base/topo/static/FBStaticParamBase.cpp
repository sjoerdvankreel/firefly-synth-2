#include <playground_base/base/topo/static/FBStaticParamBase.hpp>

using namespace juce; 

FBItemsParamNonRealTime& 
FBStaticParamBase::ItemsNonRealTime()
{
  return dynamic_cast<FBItemsParamNonRealTime&>(NonRealTime());
}

FBSteppedParamNonRealTime& 
FBStaticParamBase::SteppedNonRealTime()
{
  return dynamic_cast<FBSteppedParamNonRealTime&>(NonRealTime());
}

FBContinuousParamNonRealTime& 
FBStaticParamBase::ContinuousNonRealTime()
{
  return dynamic_cast<FBContinuousParamNonRealTime&>(NonRealTime());
}

FBItemsParamNonRealTime const& 
FBStaticParamBase::ItemsNonRealTime() const
{
  return dynamic_cast<FBItemsParamNonRealTime const&>(NonRealTime());
}

FBSteppedParamNonRealTime const& 
FBStaticParamBase::SteppedNonRealTime() const
{
  return dynamic_cast<FBSteppedParamNonRealTime const&>(NonRealTime());
}

FBContinuousParamNonRealTime const& 
FBStaticParamBase::ContinuousNonRealTime() const
{
  return dynamic_cast<FBContinuousParamNonRealTime const&>(NonRealTime());
}

float
FBStaticParamBase::DefaultNormalizedByText() const
{
  if (defaultText.size() == 0)
    return 0.0f;
  return TextToNormalized(FBValueTextDisplay::Text, defaultText).value();
}

IFBParamNonRealTime& 
FBStaticParamBase::NonRealTime()
{
  switch (type)
  {
  case FBParamType::Bars: return bars;
  case FBParamType::List: return list;
  case FBParamType::Log2: return log2;
  case FBParamType::Note: return note;
  case FBParamType::Linear: return linear;
  case FBParamType::Boolean: return boolean;
  case FBParamType::Discrete: return discrete;
  case FBParamType::DiscreteLog2: return discreteLog2;
  default: assert(false); return *static_cast<IFBParamNonRealTime*>(nullptr);
  }
}

IFBParamNonRealTime const&
FBStaticParamBase::NonRealTime() const
{
  switch (type)
  {
  case FBParamType::Bars: return bars;
  case FBParamType::List: return list;
  case FBParamType::Log2: return log2;
  case FBParamType::Note: return note;
  case FBParamType::Linear: return linear;
  case FBParamType::Boolean: return boolean;
  case FBParamType::Discrete: return discrete;
  case FBParamType::DiscreteLog2: return discreteLog2;
  default: assert(false); return *static_cast<IFBParamNonRealTime*>(nullptr);
  }
}