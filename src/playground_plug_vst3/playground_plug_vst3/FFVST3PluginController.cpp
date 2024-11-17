#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug_vst3/FFVST3PluginController.hpp>

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
  result.stepCount = param.staticTopo.stepCount;
  CopyToString128(param.name, result.title);
  CopyToString128(param.staticTopo.unit, result.units);
  CopyToString128(param.staticTopo.name, result.shortTitle);

  // TODO isList
  if (automate)
    result.flags = ParameterInfo::kCanAutomate;
  else
    result.flags = ParameterInfo::kIsReadOnly | ParameterInfo::kIsHidden;
  return result;
}

tresult PLUGIN_API
FFVST3PluginController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1;
  auto topo = FBRuntimeTopo(FFMakeTopo());
  for (int m = 0; m < topo.modules.size(); m++)
  {
    addUnit(new Unit(MakeUnitInfo(topo.modules[m], unitId)));
    for (int pp = 0; pp < topo.modules[m].plugParams.size(); pp++)
      parameters.addParameter(new Parameter(
        MakeParamInfo(topo.modules[m].plugParams[pp], unitId, false)));
    for (int ap = 0; ap < topo.modules[m].autoParams.size(); ap++)
      parameters.addParameter(new Parameter(
        MakeParamInfo(topo.modules[m].autoParams[ap], unitId, true)));
    unitId++;
  }
  return kResultTrue;
}