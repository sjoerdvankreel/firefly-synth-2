#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

FBStaticParam const&
FBSpecialParam::ParamTopo(FBStaticTopo const& topo) const
{
  return topo.modules[moduleIndex].params[paramIndex];
}

FBStaticGUIParam const&
FBSpecialGUIParam::ParamTopo(FBStaticTopo const& topo) const
{
  return topo.modules[moduleIndex].guiParams[paramIndex];
}

std::string
FBStaticTopoMeta::NameVersionAndFormat() const
{
  return name + " " + version.ToString() + " " + FBPlugFormatToString(format);
}

std::string 
FBPlugFormatToString(FBPlugFormat format)
{
  switch (format)
  {
  case FBPlugFormat::VST3: return "VST3";
  case FBPlugFormat::CLAP: return "CLAP";
  default: FB_ASSERT(false); return {};
  }
}

std::string
FBPlugVersion::ToString() const
{
  auto result = std::to_string(major);
  result += "." + std::to_string(minor);
  result += "." + std::to_string(patch);
  return result;
}