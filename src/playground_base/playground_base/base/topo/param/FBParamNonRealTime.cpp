#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

std::optional<double> 
FBParamNonRealTime::TextToNormalized(std::string const& text) const
{
  auto plain = TextToPlain(text);
  if (!plain)
    return {};
  return PlainToNormalized(plain.value());
}

std::string 
FBParamNonRealTime::NormalizedToText(FBValueTextDisplay display, double plain) const
{
  return PlainToText(display, NormalizedToPlain(plain));
}