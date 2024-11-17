#include <playground_plug/base/shared/FBPluginTopology.hpp>
#include <cstdint>

static int
UniqueIdStableHash(
  std::string const& uniqueId)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for(char c: uniqueId)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}

static std::string
RuntimeParameterNamePart(
  FBPluginStaticParameter const& staticParameter, int slot)
{
  if (staticParameter.slotCount == 1)
    return staticParameter.name;
  return staticParameter.name + " " + std::to_string(slot + 1);
}

static std::string
RuntimeModuleName(
  FBPluginStaticModule const& staticModule, int slot)
{
  if (staticModule.slotCount == 1)
    return staticModule.name;
  return staticModule.name + " " + std::to_string(slot + 1);
}

static std::string
RuntimeModuleUniqueId(
  FBPluginStaticModule const& staticModule, int slot)
{
  return staticModule.uniqueId + " " + std::to_string(slot);
}

static std::string
RuntimeParameterName(
  FBPluginStaticModule const& staticModule, int moduleSlot,
  FBPluginStaticParameter const& staticParameter, int parameterSlot)
{
  return RuntimeModuleName(staticModule, moduleSlot) + " " +
    RuntimeParameterNamePart(staticParameter, parameterSlot);
}

static std::string
RuntimeParameterUniqueId(
  FBPluginStaticModule const& staticModule, int moduleSlot,
  FBPluginStaticParameter const& staticParameter, int parameterSlot)
{
  return RuntimeModuleUniqueId(staticModule, moduleSlot) + " " +
    staticParameter.uniqueId + " " + std::to_string(parameterSlot);
}

std::unique_ptr<FBPluginRuntimeTopology>
FBGenerateRuntimeTopology(
  FBPluginStaticTopology const& staticTopology)
{
  auto result = std::make_unique<FBPluginRuntimeTopology>();

  for (int mi = 0; mi < staticTopology.modules.size(); mi++)
  {
    auto staticModule = std::make_shared<FBPluginStaticModule>(staticTopology.modules[mi]);
    for (int ms = 0; ms < staticModule->slotCount; ms++)
    {
      FBPluginRuntimeModule runtimeModule;
      runtimeModule.staticTopology = staticModule;
      runtimeModule.name = RuntimeModuleName(*staticModule, ms);
      runtimeModule.uniqueId = RuntimeModuleUniqueId(*staticModule, ms);      
      for (int pi = 0; pi < staticModule->parameters.size(); pi++)
      {
        auto staticParameter = std::make_shared<FBPluginStaticParameter>(staticModule->parameters[pi]);
        for (int ps = 0; ps < staticParameter->slotCount; ps++)
        {
          FBPluginRuntimeParameter runtimeParameter;
          runtimeParameter.staticTopology = staticParameter;
          runtimeParameter.name = RuntimeParameterName(*staticModule, ms, *staticParameter, ps);
          runtimeParameter.uniqueId = RuntimeParameterUniqueId(*staticModule, ms, *staticParameter, ps);
          runtimeParameter.uniqueIdHash = UniqueIdStableHash(runtimeParameter.uniqueId);
          runtimeModule.parameters.push_back(runtimeParameter);
        }
      }
      result->modules.push_back(runtimeModule);
    }
  }

  return result;
}