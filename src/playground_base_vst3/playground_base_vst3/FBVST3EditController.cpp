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

void 
FBVST3EditController::EndParamChange(int index)
{
  endEdit(_topo->params[index].tag);
}

void 
FBVST3EditController::BeginParamChange(int index)
{
  beginEdit(_topo->params[index].tag);
}

float
FBVST3EditController::GetParamNormalized(int index) const
{
  return parameters.getParameterByIndex(index)->getNormalized();
}

void 
FBVST3EditController::PerformParamEdit(int index, float normalized)
{
  performEdit(_topo->params[index].tag, normalized);
}

tresult PLUGIN_API 
FBVST3EditController::setParamNormalized(ParamID tag, ParamValue value)
{
  if (_guiEditor != nullptr)
    _guiEditor->SetParamNormalized(_topo->tagToParam[tag], (float)value);
  return EditControllerEx1::setParamNormalized(tag, value);
}

IPlugView* PLUGIN_API
FBVST3EditController::createView(FIDString name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  _guiEditor = new FBVST3GUIEditor(_topo->static_.guiFactory, _topo.get(), this);
  return _guiEditor;
}

tresult PLUGIN_API
FBVST3EditController::setComponentState(IBStream* state)
{
  std::string json;
  if (!FBVST3LoadIBStream(state, json))
    return kResultFalse;
  FBScalarStateContainer scalar(*_topo);
  if (!_topo->LoadState(json, scalar))
    return kResultFalse;
  for (int i = 0; i < scalar.Params().size(); i++)
  {
    float normalized = *scalar.Params()[i];
    parameters.getParameterByIndex(i)->setNormalized(normalized);
    if (_guiEditor != nullptr)
      _guiEditor->SetParamNormalized(i, normalized);
  }
  return kResultOk;
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