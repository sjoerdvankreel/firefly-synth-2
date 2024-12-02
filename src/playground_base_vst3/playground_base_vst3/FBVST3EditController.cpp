#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3Parameter.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>

#include <utility>
#include <algorithm>

static UnitInfo
MakeUnitInfo(FBRuntimeModule const& module, int id)
{
  UnitInfo result;
  result.id = id;
  result.parentUnitId = kRootUnitId;
  result.programListId = kNoProgramListId;
  FBVST3CopyToString128(module.name, result.name);
  return result;
}

static ParameterInfo
MakeParamInfo(FBRuntimeParam const& param, int unitId, bool automate)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = unitId;
  result.defaultNormalizedValue = 0.0; // TODO
  result.stepCount = std::max(0, param.static_.valueCount - 1);

  FBVST3CopyToString128(param.longName, result.title);
  FBVST3CopyToString128(param.shortName, result.shortTitle);
  FBVST3CopyToString128(param.static_.unit, result.units);

  // TODO once we drop generic editor
  if (automate)
    result.flags = ParameterInfo::kCanAutomate;
  else
    result.flags = ParameterInfo::kCanAutomate;
  return result;
}

FBVST3EditController::
FBVST3EditController(FBStaticTopo const& topo) :
_topo(std::make_unique<FBRuntimeTopo>(topo)) {}

tresult PLUGIN_API
FBVST3EditController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1;
  for (int m = 0; m < _topo->modules.size(); m++)
  {
    addUnit(new Unit(MakeUnitInfo(_topo->modules[m], unitId)));
    for (int a = 0; a < _topo->modules[m].acc.size(); a++)
    {
      auto const& topo = _topo->modules[m].acc[a];
      auto info = MakeParamInfo(topo, unitId, true);
      parameters.addParameter(new FBVST3Parameter(topo.static_, info));
    }
    for (int b = 0; b < _topo->modules[m].block.size(); b++)
    {
      auto const& topo = _topo->modules[m].block[b];
      auto info = MakeParamInfo(topo, unitId, false);
      parameters.addParameter(new FBVST3Parameter(topo.static_, info));
    }
    unitId++;
  }
  return kResultTrue;
}