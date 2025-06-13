#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBLog2Param.hpp>

bool FBLog2ParamNonRealTime::IsItems() const { return false; }
bool FBLog2ParamNonRealTime::IsStepped() const { return false; }
int FBLog2ParamNonRealTime::ValueCount() const { return 0; }
FBEditType FBLog2ParamNonRealTime::GUIEditType() const { return FBEditType::Logarithmic; }
FBEditType FBLog2ParamNonRealTime::AutomationEditType() const { return FBEditType::Logarithmic; }

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
  FB_ASSERT(0.0 <= result && result <= 1.0);
  return result;
}

double
FBLog2ParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  double result = _offset + _curveStart * std::pow(2.0, _expo * normalized);
  FB_ASSERT(result >= _curveStart + _offset);
  return result;
}

std::string
FBLog2ParamNonRealTime::PlainToText(bool io, double plain) const
{
  double displayPlain = plain * displayMultiplier;
  if (io)
    return std::to_string(displayPlain);
  return FBFormatDouble(displayPlain, FBDefaultDisplayPrecision);
}

std::optional<double>
FBLog2ParamNonRealTime::TextToPlain(bool /*io*/, std::string const& text) const
{
  auto resultOpt = FBStringToDoubleOptCLocale(text);
  if (!resultOpt)
    return std::nullopt;
  double result = resultOpt.value();
  result /= displayMultiplier;
  // account for rounding error
  double plainMin = NormalizedToPlain(0.0);
  double plainMax = NormalizedToPlain(1.0);
  if (result < plainMin * 0.99 || result > plainMax * 1.01)
    return {};
  if (result < plainMin || result > plainMax)
    FB_LOG_INFO("Clamping " + text + " to [" + std::to_string(plainMin) + ", " + std::to_string(plainMax) + "].");
  return { std::clamp(result, plainMin, plainMax) };
}