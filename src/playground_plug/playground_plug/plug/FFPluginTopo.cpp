#if 0
#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <cstdint>

std::string
FBMakeId(std::string id, int slot)
{
  return id + "-" + std::to_string(slot);
}

std::string
FBMakeName(std::string name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

int
FBMakeHash(std::string const& id)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for (char c : id)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}
#endif