#include <playground_base_vst3/FBVST3EditController.hpp>

#include <algorithm>

static void
CopyToString128(std::string const& in, String128& out)
{
  memset(out, 0, sizeof(out));
  for (int i = 0; i < 127 && i < in.size(); i++)
    out[i] = in[i];
}

static UnitInfo
MakeUnitInfo(FBRuntimeModule const& module, int id)
{
  UnitInfo result;
  result.id = id;
  result.parentUnitId = kRootUnitId;
  result.programListId = kNoProgramListId;
  CopyToString128(module.name, result.name);
  return result;
}

static ParameterInfo
MakeParamInfo(FBRuntimeParam const& param, int unitId, bool automate)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = unitId;
  result.defaultNormalizedValue = 0.0; // TODO
  result.stepCount = std::max(0, param.staticParam.valueCount - 1);

  CopyToString128(param.longName, result.title);
  CopyToString128(param.shortName, result.shortTitle);
  CopyToString128(param.staticParam.unit, result.units);

  // TODO isList
  if (automate)
    result.flags = ParameterInfo::kCanAutomate;
  else
    result.flags = ParameterInfo::kIsReadOnly | ParameterInfo::kIsHidden;
  return result;
}

FBVST3EditController::
FBVST3EditController(FBRuntimeTopo&& topo) :
_topo(std::move(topo)) {}

tresult PLUGIN_API
FBVST3EditController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1;
  for (int m = 0; m < _topo.modules.size(); m++)
  {
    addUnit(new Unit(MakeUnitInfo(_topo.modules[m], unitId)));
    for (int bp = 0; bp < _topo.modules[m].blockParams.size(); bp++)
      parameters.addParameter(new Parameter(
        MakeParamInfo(_topo.modules[m].blockParams[bp], unitId, false)));
    for (int ap = 0; ap < _topo.modules[m].accParams.size(); ap++)
      parameters.addParameter(new Parameter(
        MakeParamInfo(_topo.modules[m].accParams[ap], unitId, true)));
    unitId++;
  }
  return kResultTrue;
}