#include <playground_base/base/topo/FBStaticParam.hpp>

#include <cassert>

float 
FBStaticParam::DefaultNormalizedByText() const
{
}

std::string 
FBStaticParam::NormalizedToText(bool io, float normalized) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.PlainToText(io, list.NormalizedToPlain(normalized));
  case FBParamType::Linear:
    return linear.PlainToText(linear.NormalizedToPlain(normalized));
  case FBParamType::Boolean:
    return boolean.PlainToText(boolean.NormalizedToPlain(normalized));
  case FBParamType::Discrete:
    return discrete.PlainToText(discrete.NormalizedToPlain(normalized));
  default:
    assert(false);
    return {};
  }
}

std::optional<float> 
FBStaticParam::TextToNormalized(bool io, std::string const& text) const
{
  switch (type)
  {
    case FBParamType::List:
    {
      auto plain = list.TextToPlain(io, text);
      if (!plain) return {};
      return list.PlainToNormalized(plain.value());
    }
    case FBParamType::Boolean:
    {
      auto plain = boolean.TextToPlain(text);
      if (!plain) return {};
      return boolean.PlainToNormalized(plain.value());
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
    default:
    {
      assert(false);
      return {};
    }
  }
}