#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <cassert>

std::string
FBEditTypeToString(FBEditType type)
{
  switch (type)
  {
  case FBEditType::Linear: return "Linear";
  case FBEditType::Stepped: return "Stepped";
  case FBEditType::Logarithmic: return "Logarithmic";
  default: assert(false); return {};
  }
}

std::string
FBParamNonRealTime::NormalizedToText(bool io, double plain) const
{
  return PlainToText(io, NormalizedToPlain(plain));
}

std::optional<double> 
FBParamNonRealTime::TextToNormalized(bool io, std::string const& text) const
{
  auto plain = TextToPlain(io, text);
  if (!plain)
    return {};
  return PlainToNormalized(plain.value());
}