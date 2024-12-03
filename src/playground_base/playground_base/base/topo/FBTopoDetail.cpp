#include <playground_base/base/topo/FBTopoDetail.hpp>

std::string
FBMakeRuntimeName(
  std::string const& name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}