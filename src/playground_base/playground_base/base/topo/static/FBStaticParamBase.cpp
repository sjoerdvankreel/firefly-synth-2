#include <playground_base/base/topo/static/FBStaticParamBase.hpp>

FBParamNonRealTime const&
FBStaticParamBase::NonRealTime() const
{
  switch (type)
  {
  case FBParamType::Log2: return log2;
  case FBParamType::List: return list;
  case FBParamType::Bars: return bars;
  case FBParamType::Note: return note;
  case FBParamType::Linear: return linear;
  case FBParamType::Boolean: return boolean;
  case FBParamType::Discrete: return discrete;
  case FBParamType::DiscreteLog2: return discreteLog2;
  default: assert(false); return *static_cast<FBParamNonRealTime const*>(nullptr);
  }
}

FBItemsParamNonRealTime const&
FBStaticParamBase::ItemsNonRealTime() const
{
  switch (type)
  {
  case FBParamType::List: return list;
  case FBParamType::Bars: return bars;
  case FBParamType::Note: return note;
  default: assert(false); return *static_cast<FBItemsParamNonRealTime const*>(nullptr);
  }
}

double
FBStaticParamBase::DefaultNormalizedByText() const
{
  if (defaultText.size() == 0)
    return 0.0;
  return NonRealTime().TextToNormalized(FBTextDisplay::Text, defaultText).value();
}

std::string
FBStaticParamBase::NormalizedToTextWithUnit(FBTextDisplay display, double normalized) const
{
  std::string result = NonRealTime().NormalizedToText(display, normalized);
  if (display != FBTextDisplay::IO && !unit.empty()) // TODO is this correct ?
    result += " " + unit;
  return result;
}