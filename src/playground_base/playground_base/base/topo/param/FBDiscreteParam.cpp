#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

bool
FBDiscreteParamNonRealTime::IsItems() const
{
  return false;
}

int
FBDiscreteParamNonRealTime::ValueCount() const
{
  return valueCount;
}

float 
FBDiscreteParamNonRealTime::PlainToNormalized(int plain) const
{
  return std::clamp(plain / (valueCount - 1.0f), 0.0f, 1.0f);
}

int
FBDiscreteParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBDiscreteParamRealTime::NormalizedToPlain(normalized);
}

std::string
FBDiscreteParamNonRealTime::PlainToText(FBValueTextDisplay display, int plain) const
{
  return std::to_string(plain);
}

std::optional<int>
FBDiscreteParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return {};
  int result = static_cast<int>(plain);
  if (result < 0 || result >= valueCount)
    return { };
  return { result };
}