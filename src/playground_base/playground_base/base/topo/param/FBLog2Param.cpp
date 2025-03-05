#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBLog2Param.hpp>

bool 
FBLog2ParamNonRealTime::IsItems() const
{
  return false;
}

bool 
FBLog2ParamNonRealTime::IsStepped() const 
{
  return false;
}

int
FBLog2ParamNonRealTime::ValueCount() const 
{
  return 0;
}

FBEditType
FBLog2ParamNonRealTime::GUIEditType() const
{
  return FBEditType::Logarithmic;
}

FBEditType
FBLog2ParamNonRealTime::AutomationEditType() const
{
  return FBEditType::Logarithmic;
}

void
FBLog2Param::Init(float offset, float curveStart, float curveEnd)
{
  _offset = offset;
  _curveStart = curveStart;
  _expo = std::log(curveEnd / curveStart) / std::log(2.0f);
}

double 
FBLog2ParamNonRealTime::PlainToNormalized(double plain) const 
{
  double result = std::log2((plain - _offset) / _curveStart) / _expo;
  assert(0.0 <= result && result <= 1.0);
  return result;
}

double
FBLog2ParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  double result = _offset + _curveStart * std::pow(2.0, _expo * normalized);
  assert(result >= _curveStart + _offset);
  return result;
}

std::string
FBLog2ParamNonRealTime::PlainToText(FBTextDisplay display, double plain) const
{
  if (display == FBTextDisplay::IO)
    return std::to_string(plain);
  return FBFormatDouble(plain, FBDefaultDisplayPrecision);
}

std::optional<double>
FBLog2ParamNonRealTime::TextToPlain(FBTextDisplay display, std::string const& text) const
{
  char* end;
  double result = std::strtod(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < NormalizedToPlain(0.0) || result > NormalizedToPlain(1.0))
    return {};
  return { result };
}