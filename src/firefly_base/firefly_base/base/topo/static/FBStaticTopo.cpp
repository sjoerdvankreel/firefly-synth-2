#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

std::string
FBPlugVersion::ToString() const
{
  auto result = std::to_string(major);
  result += "." + std::to_string(minor);
  result += "." + std::to_string(patch);
  return result;
}

std::string
FBStaticTopoMeta::NameAndVersion() const
{
  return name + " " + version.ToString();
}

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