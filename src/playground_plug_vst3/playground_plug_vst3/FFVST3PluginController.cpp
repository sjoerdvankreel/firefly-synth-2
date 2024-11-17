#include <playground_plug_vst3/FFVST3PluginController.hpp>
#include <playground_plug/plug/shared/FFPluginTopology.hpp>

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

#if 0
    for (int p = 0; p < module.parameters.size(); p++)
    {
      auto const& parameter = module.parameters[p];

      ParameterInfo paramInfo = {};
      paramInfo.unitId = unitInfo.id;
      paramInfo.id = param.info.id_hash;

      auto const& param = module.params[p];
      from_8bit_string(param_info.title, param.full_name.c_str());
      from_8bit_string(param_info.shortTitle, param.full_name.c_str());
      from_8bit_string(param_info.units, param.param->domain.unit.c_str());
      param_info.defaultNormalizedValue = param.param->domain.default_normalized(module.info.slot, param.info.slot).value();

      param_info.flags = ParameterInfo::kNoFlags;
      if (param.param->dsp.can_automate(module.info.slot))
        param_info.flags |= ParameterInfo::kCanAutomate;
      else
        param_info.flags |= ParameterInfo::kIsReadOnly;
      if (param.param->gui.is_list())
        param_info.flags |= ParameterInfo::kIsList;
      param_info.stepCount = 0;
      if (!param.param->domain.is_real())
        param_info.stepCount = param.param->domain.max - param.param->domain.min;
      parameters.addParameter(new pb_param(&_automation_state, module.params[p].param, module.params[p].info.global, param_info));
    }
#endif
  }
  return kResultTrue;
}