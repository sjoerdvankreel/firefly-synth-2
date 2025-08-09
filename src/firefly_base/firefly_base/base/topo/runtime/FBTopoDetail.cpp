#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <cstdint>

std::string
FBMakeRuntimeId(
  std::string const& staticModuleId, int moduleSlot,
  std::string const& staticId, int slot)
{
  auto id = staticId + "-" + std::to_string(slot);
  auto moduleId = staticModuleId + "-" + std::to_string(moduleSlot);
  return moduleId + "-" + id;
}

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
FBMakeRuntimeModuleShortName(
  FBStaticTopo const& topo, std::string const& name,
  int slotCount, int slot,
  FBModuleSlotFormatter formatter, bool formatterOverrides)
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
FBMakeRuntimeModuleItemShortName(
  FBStaticTopo const& topo, std::string const& name,
  int moduleSlot, int itemSlotCount, int itemSlot,
  FBModuleItemSlotFormatter formatter, bool formatterOverrides)
{
  std::string result = name;
  if (formatter)
  {
    if (formatterOverrides)
      result = formatter(topo, moduleSlot, itemSlot);
    else
      result += " " + formatter(topo, moduleSlot, itemSlot);
  }
  else if (itemSlotCount > 1)
    result += " " + std::to_string(itemSlot + 1);
  return result;
}

std::string
FBMakeRuntimeModuleItemDisplayName(
  FBStaticTopo const& topo, std::string const& name, std::string const& display,
  int moduleSlot, int itemSlotCount, int itemSlot,
  FBModuleItemSlotFormatter formatter, bool formatterOverrides, bool slotFormatDisplay)
{
  if (display.empty())
    return FBMakeRuntimeModuleItemShortName(topo, name, moduleSlot, itemSlotCount, itemSlot, formatter, formatterOverrides);
  if(slotFormatDisplay)
    return FBMakeRuntimeModuleItemShortName(topo, display, moduleSlot, itemSlotCount, itemSlot, formatter, formatterOverrides);
  return display;
}