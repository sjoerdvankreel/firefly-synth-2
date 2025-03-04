#include <playground_base/base/topo/static/FBStaticParamBase.hpp>

using namespace juce;

PopupMenu
FBStaticParamBase::MakePopupMenu() const
{
  switch (type)
  {
  case FBParamType::List:
    return List().MakePopupMenu();
  case FBParamType::Note:
    return Note().MakePopupMenu();
  case FBParamType::Bars:
    return Bars().MakePopupMenu();
  default:
    assert(false);
    return {};
  }
}

float
FBStaticParamBase::DefaultNormalizedByText() const
{
  if (defaultText.size() == 0)
    return 0.0f;
  return TextToNormalized(FBValueTextDisplay::Text, defaultText).value();
}

std::string
FBStaticParamBase::NormalizedToText(FBParamTextDisplay display, float normalized) const
{
  std::string result = NormalizedToText((FBValueTextDisplay)display, normalized);
  if (display == FBParamTextDisplay::TooltipWithUnit && !unit.empty())
    result += " " + unit;
  return result;
}

float
FBStaticParamBase::AnyDiscreteToNormalizedSlow(int plain) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.PlainToNormalized(plain);
  case FBParamType::Note:
    return note.PlainToNormalized(plain);
  case FBParamType::Bars:
    return bars.PlainToNormalized(plain);
  case FBParamType::Boolean:
    return boolean.PlainToNormalized(plain);
  case FBParamType::Discrete:
    return discrete.PlainToNormalized(plain);
  case FBParamType::DiscreteLog2:
    return discreteLog2.PlainToNormalized(plain);
  default:
    assert(false);
    return {};
  }
}

int 
FBStaticParamBase::NormalizedToAnyDiscreteSlow(float normalized) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.NormalizedToPlain(normalized);
  case FBParamType::Note:
    return note.NormalizedToPlain(normalized);
  case FBParamType::Bars:
    return bars.NormalizedToPlain(normalized);
  case FBParamType::Boolean:
    return boolean.NormalizedToPlain(normalized);
  case FBParamType::Discrete:
    return discrete.NormalizedToPlain(normalized);
  case FBParamType::DiscreteLog2:
    return discreteLog2.NormalizedToPlain(normalized);
  default:
    assert(false);
    return {};
  }
}

std::string 
FBStaticParamBase::NormalizedToText(FBValueTextDisplay display, float normalized) const
{
  switch (type)
  {
  case FBParamType::Note:
    return note.PlainToText(note.NormalizedToPlain(normalized));
  case FBParamType::Bars:
    return bars.PlainToText(bars.NormalizedToPlain(normalized));
  case FBParamType::Boolean:
    return boolean.PlainToText(display, boolean.NormalizedToPlain(normalized));
  case FBParamType::Discrete:
    return discrete.PlainToText(discrete.NormalizedToPlain(normalized));
  case FBParamType::List:
    return list.PlainToText(display, list.NormalizedToPlain(normalized));
  case FBParamType::Log2:
    return log2.PlainToText(display, log2.NormalizedToPlain(normalized));
  case FBParamType::Linear:
    return linear.PlainToText(display, linear.NormalizedToPlain(normalized));
  case FBParamType::DiscreteLog2:
    return discreteLog2.PlainToText(discreteLog2.NormalizedToPlain(normalized));
  default:
    assert(false);
    return {};
  }
}

std::optional<float> 
FBStaticParamBase::TextToNormalized(FBValueTextDisplay display, std::string const& text) const
{
  switch (type)
  {
    case FBParamType::Note:
    {
      auto plain = note.TextToPlain(text);
      if (!plain) return {};
      return note.PlainToNormalized(plain.value());
    }
    case FBParamType::List:
    {
      auto plain = list.TextToPlain(display == FBValueTextDisplay::IO, text);
      if (!plain) return {};
      return list.PlainToNormalized(plain.value());
    }
    case FBParamType::Bars:
    {
      auto plain = bars.TextToPlain(text);
      if (!plain) return {};
      return bars.PlainToNormalized(plain.value());
    }
    case FBParamType::Boolean:
    {
      auto plain = boolean.TextToPlain(display, text);
      if (!plain) return {};
      return boolean.PlainToNormalized(plain.value());
    }
    case FBParamType::Log2:
    {
      auto plain = log2.TextToPlain(text);
      if (!plain) return {};
      return log2.PlainToNormalized(plain.value());
    }
    case FBParamType::Linear:
    {
      auto plain = linear.TextToPlain(text);
      if (!plain) return {};
      return linear.PlainToNormalized(plain.value());
    }
    case FBParamType::Discrete:
    {
      auto plain = discrete.TextToPlain(text);
      if (!plain) return {};
      return discrete.PlainToNormalized(plain.value());
    }
    case FBParamType::DiscreteLog2:
    {
      auto plain = discreteLog2.TextToPlain(text);
      if (!plain) return {};
      return discreteLog2.PlainToNormalized(plain.value());
    }
    default:
    {
      assert(false);
      return {};
    }
  }
}