#include <playground_base/base/topo/param/FBFreqOctParam.hpp>

double
FBFreqOctParam::PlainToNormalized(double plain) const
{
  return std::log2(plain / minHz) / octaves;
}

std::string
FBFreqOctParam::PlainToText(double plain) const
{
  return std::to_string(plain);
}

std::optional<double>
FBFreqOctParam::TextToPlain(std::string const& text) const
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < minHz || result > NormalizedToPlain(1.0))
    return {};
  return { result };
}