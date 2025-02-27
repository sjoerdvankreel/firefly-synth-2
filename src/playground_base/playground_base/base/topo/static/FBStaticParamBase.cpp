#include <playground_base/base/topo/static/FBStaticParamBase.hpp>

using namespace juce;

IFBListParamNonRealTime&
FBStaticParamBase::ListNonRealTime()
{
  return dynamic_cast<IFBListParamNonRealTime&>(NonRealTime());
}

IFBListParamNonRealTime const&
FBStaticParamBase::ListNonRealTime() const
{
  return dynamic_cast<IFBListParamNonRealTime const&>(NonRealTime());
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

// TODO// TODO// TODO// TODO// TODO// TODO// TODO

std::string 
FBStaticParamBase::NormalizedToText(FBParamTextDisplay display, float normalized) const
{

  std::string result = NonRealTime().NormalizedToText((FBValueTextDisplay)display, normalized);
  if (display == FBParamTextDisplay::TooltipWithUnit && !unit.empty())
    result += " " + unit;
  return result;
}

std::optional<float>
FBStaticParamBase::TextToNormalized(FBValueTextDisplay display, std::string const& text) const
{
}

float
FBStaticParamBase::DefaultNormalizedByText() const
{
}

std::string
FBStaticParamBase::NormalizedToText(FBParamTextDisplay display, float normalized) const
{
}