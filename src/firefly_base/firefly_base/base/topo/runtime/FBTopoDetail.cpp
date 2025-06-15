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
FBMakeRuntimeTabName(
  std::string const& name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

std::string
FBMakeRuntimeGraphName(
  std::string const& name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += std::to_string(slot + 1);
  return result;
}

std::string
FBMakeRuntimeShortName(
  FBStaticTopo const& topo, std::string const& name, 
  int slotCount, int slot, 
  FBParamSlotFormatter formatter, bool formatterOverrides)
{
  std::string result = name;
  if (formatter)
  {
    if (formatterOverrides)
      result = formatter(topo, slot);
    else
      result += " " + formatter(topo, slot);
  }
  else if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

std::string
FBMakeRuntimeDisplayName(
  FBStaticTopo const& topo, std::string const& name, std::string const& display,
  int slotCount, int slot, FBParamSlotFormatter formatter, bool formatterOverrides)
{
  if (!display.empty())
    return display;
  return FBMakeRuntimeShortName(topo, name, slotCount, slot, formatter, formatterOverrides);
}