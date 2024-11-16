#include <playground_plug/base/shared/FBPluginTopology.hpp>

static std::string
RuntimeParameterNamePart(
  FBPluginStaticParameterBase const& staticParameter, int slot)
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
  FBPluginStaticParameterBase const& staticParameter, int parameterSlot)
{
  return RuntimeModuleName(staticModule, moduleSlot) + " " +
    RuntimeParameterNamePart(staticParameter, parameterSlot);
}

static std::string
RuntimeParameterUniqueId(
  FBPluginStaticModule const& staticModule, int moduleSlot,
  FBPluginStaticParameterBase const& staticParameter, int parameterSlot)
{
  return RuntimeModuleUniqueId(staticModule, moduleSlot) + " " +
    staticParameter.uniqueId + " " + std::to_string(parameterSlot);
}

static FBPluginRuntimeParameter
MakeRuntimeParameter(
  FBPluginStaticModule const& staticModule, int ms, 
  std::shared_ptr<FBPluginStaticParameterBase> staticParameter, int ps, 
  int index, int stepCount)
{
  FBPluginRuntimeParameter result;
  result.index = index;
  result.stepCount = stepCount;
  result.staticTopology = staticParameter;
  result.name = RuntimeParameterName(staticModule, ms, *staticParameter, ps);
  result.uniqueId = RuntimeParameterUniqueId(staticModule, ms, *staticParameter, ps);
  return result;
}

std::unique_ptr<FBPluginRuntimeTopology>
FBGenerateRuntimeTopology(
  FBPluginStaticTopology const& staticTopology)
{
  auto result = std::make_unique<FBPluginRuntimeTopology>();

  int runtimeModuleIndex = 0;
  int runtimeParameterIndex = 0;
  for (int mi = 0; mi < staticTopology.modules.size(); mi++)
  {
    auto staticModule = std::make_shared<FBPluginStaticModule>(staticTopology.modules[mi]);
    for (int ms = 0; ms < staticModule->slotCount; ms++)
    {
      FBPluginRuntimeModule runtimeModule;
      runtimeModule.index = runtimeModuleIndex++;
      runtimeModule.staticTopology = staticModule;
      runtimeModule.name = RuntimeModuleName(*staticModule, ms);
      runtimeModule.uniqueId = RuntimeModuleUniqueId(*staticModule, ms);      
      for (int dpi = 0; dpi < staticModule->discreteParameters.size(); dpi++)
      {
        auto staticParameter = std::make_shared<FBPluginStaticDiscreteParameter>(staticModule->discreteParameters[dpi]);
        for (int dps = 0; dps < staticParameter->slotCount; dps++)
        {
          FBPluginRuntimeParameter runtimeParameter = MakeRuntimeParameter(
            *staticModule, ms, 
            staticParameter, dps, 
            runtimeParameterIndex++, staticParameter->stepCount);
          runtimeModule.parameters.push_back(runtimeParameter);
        }
      }
      for (int cpi = 0; cpi < staticModule->continuousParameters.size(); cpi++)
      {
        auto staticParameter = std::make_shared<FBPluginStaticContinuousParameter>(staticModule->continuousParameters[cpi]);
        for (int cps = 0; cps < staticParameter->slotCount; cps++)
        {
          FBPluginRuntimeParameter runtimeParameter = MakeRuntimeParameter(
            *staticModule, ms,
            staticParameter, cps,
            runtimeParameterIndex++, 0);
          runtimeModule.parameters.push_back(runtimeParameter);
        }
      }
      result->modules.push_back(runtimeModule);
    }
  }

  return result;
}