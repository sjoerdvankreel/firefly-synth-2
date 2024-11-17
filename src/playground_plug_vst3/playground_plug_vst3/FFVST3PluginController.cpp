#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug_vst3/FFVST3PluginController.hpp>

static UnitInfo
MakeUnitInfo(FBRuntimeModule const& module, int id)
{
  UnitInfo result;
  result.id = id;
  result.parentUnitId = kRootUnitId;
  result.programListId = kNoProgramListId;
  memset(result.name, 0, sizeof(result.name));
  for (int i = 0; i < 127 && i < module.name.size(); i++)
    result.name[i] = module.name[i];
  return result;
}

tresult PLUGIN_API
FFVST3PluginController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1; // 0 is root
  auto topo = FBRuntimeTopo(FFMakeTopo());
  for (int m = 0; m < topo.modules.size(); m++)
    addUnit(new Unit(MakeUnitInfo(topo.modules[m], unitId++)));
  return kResultTrue;
}