#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3Parameter.hpp>
#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <base/source/fstring.h>
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
  result.stepCount = std::max(0, param.static_.ValueCount() - 1);
  result.defaultNormalizedValue = param.static_.DefaultNormalizedByText();

  FBVST3CopyToString128(param.longName, result.title);
  FBVST3CopyToString128(param.shortName, result.shortTitle);
  FBVST3CopyToString128(param.static_.unit, result.units);

  // TODO once we drop generic editor
  result.flags = ParameterInfo::kCanAutomate;
  if (param.static_.type == FBParamType::List)
    result.flags |= ParameterInfo::kIsList;
  return result;
}

FBVST3EditController::
FBVST3EditController(FBStaticTopo const& topo) :
_topo(std::make_unique<FBRuntimeTopo>(topo)) {}

IPlugView* PLUGIN_API
FBVST3EditController::createView(FIDString name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  return new FBVST3GUIEditor(_topo->static_.guiFactory, this);
}

float
FBVST3EditController::GetParamNormalized(int index) const
{
  return parameters.getParameterByIndex(index)->getNormalized();
}

void 
FBVST3EditController::SetParamNormalized(int index, float normalized)
{
  parameters.getParameterByIndex(index)->setNormalized(normalized);
}

tresult PLUGIN_API
FBVST3EditController::setState(IBStream* state)
{
  std::string json;
  if (!FBVST3LoadIBStream(state, json))
    return kResultFalse;
  FBScalarStateContainer scalar(*_topo);
  if (!_topo->LoadState(json, scalar))
    return kResultFalse;
  for (int i = 0; i < scalar.Params().size(); i++)
    parameters.getParameterByIndex(i)->setNormalized(*scalar.Params()[i]);
  return kResultOk;
}

tresult PLUGIN_API
FBVST3EditController::getState(IBStream* state)
{
  FBScalarStateContainer scalar(*_topo);
  for (int i = 0; i < scalar.Params().size(); i++)
    *scalar.Params()[i] = parameters.getParameterByIndex(i)->getNormalized();
  std::string json = _topo->SaveState(scalar);
  bool result = FBVST3SaveIBStream(state, json);
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