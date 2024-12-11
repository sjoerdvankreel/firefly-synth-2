#include <playground_base/base/topo/FBRuntimeTopo.hpp>
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
MakeParamInfo(FBRuntimeParam const& param, int unitId)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = unitId;
  result.stepCount = std::max(0, param.static_.valueCount - 1);
  result.defaultNormalizedValue = param.static_.defaultNormalized;

  FBVST3CopyToString128(param.longName, result.title);
  FBVST3CopyToString128(param.shortName, result.shortTitle);
  FBVST3CopyToString128(param.static_.unit, result.units);

  // TODO once we drop generic editor
  result.flags = ParameterInfo::kCanAutomate;
  if (param.static_.list.size()) 
    result.flags |= ParameterInfo::kIsList;
  return result;
}

FBVST3EditController::
FBVST3EditController(FBStaticTopo const& topo) :
_topo(std::make_unique<FBRuntimeTopo>(topo)) {}

tresult PLUGIN_API
FBVST3EditController::setState(IBStream* state)
{
  std::string json;
  if (!FBVST3LoadIBStream(state, json))
    return kResultFalse;
  void* scalar = _topo->static_.allocScalarState();
  auto ptrs = _topo->MakeScalarStatePtrs(scalar);
  bool result = _topo->LoadState(json, ptrs);
  if (result)
    for (int i = 0; i < ptrs.Params().size(); i++)
      parameters.getParameterByIndex(i)->setNormalized(*ptrs.Params()[i]);
  _topo->static_.freeScalarState(scalar);
  return result? kResultOk: kResultFalse;
}

tresult PLUGIN_API
FBVST3EditController::getState(IBStream* state)
{
  void* scalar = _topo->static_.allocScalarState();
  auto ptrs = _topo->MakeScalarStatePtrs(scalar);
  for (int i = 0; i < ptrs.Params().size(); i++)
    *ptrs.Params()[i] = parameters.getParameterByIndex(i)->getNormalized();
  std::string json = _topo->SaveState(ptrs);
  bool result = FBVST3SaveIBStream(state, json);
  _topo->static_.freeScalarState(scalar);
  return result ? kResultOk : kResultFalse;
}

tresult PLUGIN_API
FBVST3EditController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1;
  for (int m = 0; m < _topo->modules.size(); m++)
  {
    addUnit(new Unit(MakeUnitInfo(_topo->modules[m], unitId)));
    for (int p = 0; p < _topo->modules[m].params.size(); p++)
    {
      auto const& topo = _topo->modules[m].params[p];
      auto info = MakeParamInfo(topo, unitId);
      parameters.addParameter(new FBVST3Parameter(topo.static_, info));
    }
    unitId++;
  }
  return kResultTrue;
}