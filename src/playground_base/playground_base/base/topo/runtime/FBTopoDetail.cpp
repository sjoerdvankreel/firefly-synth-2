#include <playground_base/base/topo/runtime/FBTopoDetail.hpp>
#include <cstdint>

int
FBMakeStableHash(std::string const& id)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for (char c : id)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}

std::string
FBMakeRuntimeName(std::string const& name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

std::string
FBMakeRuntimeTooltip(std::string const& name, std::string const& tooltip, int slotCount, int slot)
{
  if (tooltip.empty())
    return FBMakeRuntimeName(name, slotCount, slot);
  return FBMakeRuntimeName(tooltip, slotCount, slot);
}