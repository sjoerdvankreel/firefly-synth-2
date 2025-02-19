#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBLog2Param.hpp>

float
FBLog2Param::PlainToNormalized(float plain) const
{
  return std::log2(plain) / (max - min) - min;
}

std::string
FBLog2Param::PlainToText(FBValueTextDisplay display, float plain) const
{
  if (display == FBValueTextDisplay::IO)
    return std::to_string(plain);
  return FBFormatFloat(plain, FBDefaultDisplayPrecision);
}

std::optional<float>
FBLog2Param::TextToPlain(std::string const& text) const
{
  char* end;
  float result = std::strtof(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < min || result > NormalizedToPlain(1.0f))
    return {};
  return { result };
}