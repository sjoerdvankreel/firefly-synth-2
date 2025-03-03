#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

std::string 
FBParamNonRealTime::NormalizedToText(FBValueTextDisplay display, double normalized) const
{
  return PlainToText(display, NormalizedToPlain(normalized));
}

std::optional<double> 
FBParamNonRealTime::TextToNormalized(FBValueTextDisplay display, std::string const& text) const
{
  auto result = TextToPlain(display, text);
  if (!result)
    return result;
  return { PlainToNormalized(result.value()) };
}