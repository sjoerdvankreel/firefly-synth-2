#include <playground_base/base/topo/param/FBFreqOctParam.hpp>
#include <format>

float
FBFreqOctParam::PlainToNormalized(float plain) const
{
  return std::log2(plain / minHz) / octaves;
}

std::string
FBFreqOctParam::PlainToText(FBTextDisplay display, float plain) const
{
  if(display == FBTextDisplay::IO)
    return std::to_string(plain);
  return std::format("{:.3f}", plain);
} 

std::optional<float>
FBFreqOctParam::TextToPlain(std::string const& text) const
{
  char* end;
  float result = std::strtof(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < minHz || result > NormalizedToPlain(1.0f))
    return {};
  return { result };
}