#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

std::string
FBParamNonRealTime::NormalizedToText(FBTextDisplay display, double plain) const
{
  return PlainToText(display, NormalizedToPlain(plain));
}

std::optional<double> 
FBParamNonRealTime::TextToNormalized(FBTextDisplay display, std::string const& text) const
{
  auto plain = TextToPlain(display, text);
  if (!plain)
    return {};
  return PlainToNormalized(plain.value());
}