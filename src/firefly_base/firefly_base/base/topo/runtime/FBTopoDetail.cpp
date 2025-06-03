#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
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
FBMakeRuntimeShortName(
  std::string const& name, int slotCount, 
  int slot, FBParamSlotFormatter formatter)
{
  std::string result = name;
  if (formatter)
    result += " " + formatter(slot);
  else if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

std::string
FBMakeRuntimeDisplayName(
  std::string const& name, std::string const& display, 
  int slotCount, int slot, FBParamSlotFormatter formatter)
{
  if (!display.empty())
    return display;
  return FBMakeRuntimeShortName(name, slotCount, slot, formatter);
}