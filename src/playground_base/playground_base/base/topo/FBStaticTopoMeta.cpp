#include <playground_base/base/topo/FBStaticTopoMeta.hpp>

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