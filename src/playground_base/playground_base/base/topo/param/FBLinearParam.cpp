#include <playground_base/base/topo/param/FBLinearParam.hpp>

double
FBLinearParam::PlainToNormalized(double plain) const
{
  return std::clamp((plain - min) / (max - min), 0.0, 1.0);
}

std::string
FBLinearParam::PlainToText(double plain) const
{
  return std::to_string(plain * displayMultiplier);
}

std::optional<double>
FBLinearParam::TextToPlain(std::string const& text) const
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  result /= displayMultiplier;
  if (result < min || result > max)
    return {};
  return { result };
}