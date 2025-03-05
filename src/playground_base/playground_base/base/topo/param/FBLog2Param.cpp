#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBLog2Param.hpp>

float
FBLog2Param::PlainToNormalized(float plain) const
{
  float result = std::log2((plain - _offset) / _curveStart) / _expo;
  assert(0.0f <= result && result <= 1.0f);
  return result;
}

void 
FBLog2Param::Init(float offset, float curveStart, float curveEnd)
{
  _offset = offset;
  _curveStart = curveStart;
  _expo = std::log(curveEnd / curveStart) / std::log(2.0f);
}

std::string
FBLog2Param::PlainToText(FBValueTextDisplay display, float plain) const
{
  if(display == FBValueTextDisplay::IO)
    return std::to_string(plain);
  return FBFormatDouble(plain, FBDefaultDisplayPrecision);
} 

std::optional<float>
FBLog2Param::TextToPlain(std::string const& text) const
{
  char* end;
  float result = std::strtof(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < NormalizedToPlain(0.0f) || result > NormalizedToPlain(1.0f))
    return {};
  return { result };
}