#include <playground_plug_vst3/FFVST3PluginController.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>

tresult PLUGIN_API
FFVST3PluginController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  auto staticTopology = FFCreateStaticTopology();
  auto runtimeTopology = FBGenerateRuntimeTopology(*staticTopology);

  int unitId = 1; // 0 is root
  for (int m = 0; m < runtimeTopology->modules.size(); m++)
  {
    UnitInfo unitInfo;
    unitInfo.id = unitId++;
    unitInfo.parentUnitId = kRootUnitId;
    unitInfo.programListId = kNoProgramListId;
    memset(unitInfo.name, 0, sizeof(unitInfo.name));
    auto const& module = runtimeTopology->modules[m];
    for (int i = 0; i < 127 && i < module.name.size(); i++)
      unitInfo.name[i] = module.name[i];
    addUnit(new Unit(unitInfo));
  }
  return kResultTrue;
}