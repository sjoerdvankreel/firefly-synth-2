#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>
#include <firefly_base/base/topo/static/FBParamNonRealTime.hpp>

std::string
FBEditTypeToString(FBEditType type)
{
  switch (type)
  {
  case FBEditType::Linear: return "Linear";
  case FBEditType::Stepped: return "Stepped";
  case FBEditType::Logarithmic: return "Logarithmic";
  default: FB_ASSERT(false); return {};
  }
}

std::string
FBParamNonRealTime::NormalizedToText(bool io, int moduleIndex, double normalized) const
{
  return PlainToText(io, moduleIndex, NormalizedToPlain(normalized));
}

std::optional<double> 
FBParamNonRealTime::TextToPlain(FBStaticParamBase const& param, bool io, int moduleIndex, std::string const& text) const
{
  auto result = TextToPlainInternal(io, moduleIndex, text);
  if (result)
    return result;
  FB_LOG_INFO("Failed to parse text '" + text + "' for param '" + param.DebugNameAndId() + "'.");
  return {};
}

std::optional<double> 
FBParamNonRealTime::TextToNormalized(FBStaticParamBase const& param, bool io, int moduleIndex, std::string const& text) const
{
  auto plain = TextToPlain(param, io, moduleIndex, text);
  if (!plain)
    return {};
  return PlainToNormalized(plain.value());
}