#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <cstdint>

static std::string
MakeId(std::string id, int slot)
{
  return id + "-" + std::to_string(slot);
}

static std::string
MakeName(std::string name, int slotCount, int slot)
{
  std::string result = name;
  if (slotCount > 1)
    result += " " + std::to_string(slot + 1);
  return result;
}

static int
MakeHash(std::string const& id)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for (char c : id)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& module, int moduleSlot,
  FBStaticParam const& param, int paramSlot)
{
  staticTopo = param;
  id = MakeId(module.id, moduleSlot);
  id += "-" + MakeId(param.id, paramSlot);
  name = MakeName(module.name, module.slotCount, moduleSlot);
  name += MakeName(param.name, param.slotCount, paramSlot);
  tag = MakeHash(id);
}

FBRuntimeTopo::
FBRuntimeTopo(
  FBStaticTopo const& topo)
{
  for (int mi = 0; mi < topo.modules.size(); mi++)
    for (int ms = 0; ms < topo.modules[mi].slotCount; ms++)
      modules.push_back(FBRuntimeModule(topo.modules[mi], ms));
}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticModule const& module, int slot)
{
  id = MakeId(module.id, slot);
  name = MakeName(module.name, module.slotCount, slot);
  for (int ppi = 0; ppi < module.plugParams.size(); ppi++)
    for (int pps = 0; pps < module.plugParams[ppi].slotCount; pps++)
      plugParams.push_back(FBRuntimeParam(module, slot, module.plugParams[ppi], pps));
  for (int api = 0; api < module.autoParams.size(); api++)
    for (int aps = 0; aps < module.autoParams[api].slotCount; aps++)
      plugParams.push_back(FBRuntimeParam(module, slot, module.autoParams[api], aps));
}