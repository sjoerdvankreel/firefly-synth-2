#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeCVOutput.hpp>

static std::string
MakeRuntimeParamModName(
  FBStaticTopo const& topo,
  FBStaticModule const& module,
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeShortName(topo, param.name, param.slotCount, indices.param.slot, param.slotFormatter, param.slotFormatterOverrides);
  auto moduleName = FBMakeRuntimeShortName(topo, module.name, module.slotCount, indices.module.slot, {}, false);
  return moduleName + " " + paramName;
}

std::vector<FBParamTopoIndices> 
FFModMatrixMakeTargets(bool global, FBStaticTopo const* topo)
{
  std::vector<FBParamTopoIndices> result = {};
  result.push_back({ { -1, -1 }, { -1, -1 } });
  for (int m = 0; m < topo->modules.size(); m++)
  {
    auto const& module = topo->modules[m];
    if (global != module.voice)
      for (int ms = 0; ms < module.slotCount; ms++)
        for (int p = 0; p < module.params.size(); p++)
        {
          auto const& param = module.params[p];
          if (param.AutomationTiming() == FBAutomationTiming::PerSample)
            for (int ps = 0; ps < param.slotCount; ps++)
              result.push_back({ { m, ms }, { p, ps } });
        }
  }
  return result;
}

std::vector<FFModMatrixSource>
FFModMatrixMakeSources(bool global, FBStaticTopo const* topo)
{
  std::vector<FFModMatrixSource> result = {};
  for (int m = 0; m < topo->modules.size(); m++)
  {
    auto const& module = topo->modules[m];
    if (!module.voice || !global)
      for (int ms = 0; ms < module.slotCount; ms++)
        for (int o = 0; o < module.cvOutputs.size(); o++)
        {
          auto const& cvOutput = module.cvOutputs[o];
          for (int os = 0; os < cvOutput.slotCount; os++)
            result.push_back({ false, { { m, ms }, { o, os } } });
        }
  }
  if (!global)
  {
    for (int m = 0; m < topo->modules.size(); m++)
    {
      auto const& module = topo->modules[m];
      if (!module.voice)
        for (int ms = 0; ms < module.slotCount; ms++)
          for (int o = 0; o < module.cvOutputs.size(); o++)
          {
            auto const& cvOutput = module.cvOutputs[o];
            for (int os = 0; os < cvOutput.slotCount; os++)
              result.push_back({ true, { { m, ms }, { o, os } } });
          }
    }
  }
  return result;
}

std::unique_ptr<FBStaticModule>
FFMakeModMatrixTopo(bool global, FFStaticTopo const* topo)
{
  int slotCount = global ? FFModMatrixGlobalSlotCount : FFModMatrixVoiceSlotCount;
  std::string prefix = global ? "G" : "V";
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->slotCount = 1;
  result->name = global ? "GMatrix" : "VMatrix";
  result->id = prefix + "{19758BF4-241B-4A59-A943-3AE7426C7CC9}";
  result->params.resize((int)FFModMatrixParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vMatrix; });
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gMatrix; });
  auto selectVoiceModule = [](auto& state) { return &state.voice.vMatrix; };
  auto selectGlobalModule = [](auto& state) { return &state.global.gMatrix; };

  auto& opType = result->params[(int)FFModMatrixParam::OpType];
  opType.acc = false;
  opType.name = "Op";
  opType.display = "Op";
  opType.slotCount = slotCount;
  opType.defaultText = "Off";
  opType.id = prefix + "{8D28D968-8585-4A4D-B636-F365C5873973}";
  opType.type = FBParamType::List;
  opType.List().items = {
    { "{8E7F2BE6-12B7-483E-8308-DD96F63C7743}", "Off" },
    { "{33CE627C-A02D-43C0-A533-257E4D03EA1E}", "Add" },
    { "{F01ABE4C-C22E-47F2-900E-7E913906A740}", "Mul" },
    { "{91B784D0-E47A-46DC-ACD8-15A502E68A9A}", "Stack" },
    { "{23F72708-1F63-4AAB-9970-9F1D77FC5245}", "BP Add" },
    { "{98709D78-A6A9-4836-A64A-50B30167497B}", "BP Stk" } };
  auto selectOpType = [](auto& module) { return &module.block.opType; };
  opType.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOpType);
  opType.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOpType);
  opType.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOpType);
  opType.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOpType);
  opType.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOpType);

  auto& amount = result->params[(int)FFModMatrixParam::Amount];
  amount.acc = true;
  amount.defaultText = "100";
  amount.name = "Amount";
  amount.display = "Amt";
  amount.slotCount = slotCount;
  amount.unit = "%";
  amount.id = prefix + "{880BC229-2794-45CC-859E-608E85A51D72}";
  amount.type = FBParamType::Identity;
  auto selectAmount = [](auto& module) { return &module.acc.amount; };
  amount.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectAmount);
  amount.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectAmount);
  amount.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectAmount);
  amount.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectAmount);
  amount.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectAmount);
  amount.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });

  auto& source = result->params[(int)FFModMatrixParam::Source];
  source.acc = false;
  source.name = "Source";
  source.display = "Source";
  source.slotCount = slotCount;
  source.id = prefix + "{08DB9477-1B3A-4EC8-88C9-AF3A9ABA9CD8}";
  source.type = FBParamType::List;
  source.List().linkedTarget = (int)FFModMatrixParam::Target;
  auto selectSource = [](auto& module) { return &module.block.source; };
  source.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSource);
  source.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSource);
  source.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSource);
  source.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSource);
  source.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSource);
  source.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });
  
  auto const& sources = global ? topo->gMatrixSources : topo->vMatrixSources;
  for (int i = 0; i < sources.size(); i++)
  {
    int moduleSlot = sources[i].indices.module.slot;
    int cvOutputSlot = sources[i].indices.cvOutput.slot;
    auto const& module = topo->modules[sources[i].indices.module.index];
    auto const& cvOutput = module.cvOutputs[sources[i].indices.cvOutput.index];
    std::string onNoteIdPrefix = sources[i].onNote ? (FBOnNotePrefix + "-") : "";
    std::string onNoteNamePrefix = sources[i].onNote ? (FBOnNotePrefix + " ") : "";
    if (moduleSlot == 0)
      source.List().submenuStart[i] = onNoteNamePrefix + module.name;
    auto id = FBMakeRuntimeId(module.id, moduleSlot, onNoteIdPrefix + cvOutput.id, cvOutputSlot);
    auto name = FBMakeRuntimeCVOutputName(*topo, module, cvOutput, sources[i].indices, sources[i].onNote);
    source.List().items.push_back({ id, name });
  }

  auto& target = result->params[(int)FFModMatrixParam::Target];
  target.acc = false;
  target.name = "Target";
  target.display = "Target";
  target.slotCount = slotCount;
  target.id = prefix + "{DB2C381F-7CA5-49FA-83C1-93DFECF9F97C}";
  target.type = FBParamType::List;
  target.List().linkedSource = (int)FFModMatrixParam::Source;
  auto selectTarget = [](auto& module) { return &module.block.target; };
  target.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTarget);
  target.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTarget);
  target.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTarget);
  target.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTarget);
  target.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTarget);
  target.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });

  FBTopoIndices prevModule = { -1, -1 };
  auto const& targets = global ? topo->gMatrixTargets : topo->vMatrixTargets;
  target.List().submenuStart[0] = "Off";
  target.List().items.push_back({ "{068FDE01-F1B1-4A4D-83FF-BD8B3D4E43C9}", "Off" });
  for (int i = 1; i < targets.size(); i++)
  {
    int paramSlot = targets[i].param.slot;
    int moduleSlot = targets[i].module.slot;
    auto const& module = topo->modules[targets[i].module.index];
    auto const& param = module.params[targets[i].param.index];
    if (targets[i].module != prevModule)
    {
      std::string subMenuName = module.name;
      if (module.slotCount != 1)
        subMenuName += " " + std::to_string(moduleSlot + 1);
      target.List().submenuStart[i] = subMenuName;
      prevModule = targets[i].module;
    }
    auto id = FBMakeRuntimeId(module.id, moduleSlot, param.id, paramSlot);
    auto name = MakeRuntimeParamModName(*topo, module, param, targets[i]);
    target.List().items.push_back({ id, name });
  }

  target.List().enabledSelector = [global](FBHostGUIContext const* context, int runtimeTargetValue)
  {
    if (runtimeTargetValue == 0)
      return true; // Off
    auto const& topo = dynamic_cast<FFStaticTopo const&>(*context->Topo()->static_);
    auto const& targets = global ? topo.gMatrixTargets : topo.vMatrixTargets;
    auto const& targetParamIndices = targets[runtimeTargetValue];
    auto const& targetParam = context->Topo()->audio.ParamAtTopo(targetParamIndices);
    FBParamsDependentDependency visible(
      context->Topo(), targetParamIndices.param.slot, true, 
      targetParamIndices.module, targetParam->static_.dependencies.visible.audio);
    FBParamsDependentDependency enabled(
      context->Topo(), targetParamIndices.param.slot, true,
      targetParamIndices.module, targetParam->static_.dependencies.enabled.audio);
    return visible.EvaluateAudio(context) && enabled.EvaluateAudio(context);
  };

  target.List().linkedTargetEnabledSelector = [global, topo](int runtimeSourceValue, int runtimeTargetValue) {
    if (runtimeTargetValue == 0)
      return true; // Off
    auto const& targetParams = global ? topo->gMatrixTargets : topo->vMatrixTargets;
    auto const& sourceCvOutputs = global ? topo->gMatrixSources : topo->vMatrixSources;
    FB_ASSERT(0 <= runtimeTargetValue && runtimeTargetValue < targetParams.size());
    FB_ASSERT(0 <= runtimeSourceValue && runtimeSourceValue < sourceCvOutputs.size());
    auto const& targetParam = targetParams[runtimeTargetValue];
    auto const& sourceCvOutput = sourceCvOutputs[runtimeSourceValue];
    auto targetProcessIter = std::find(topo->moduleProcessOrder.begin(), topo->moduleProcessOrder.end(), targetParam.module);
    auto sourceProcessIter = std::find(topo->moduleProcessOrder.begin(), topo->moduleProcessOrder.end(), sourceCvOutput.indices.module);
    FB_ASSERT(targetProcessIter != topo->moduleProcessOrder.end());
    FB_ASSERT(sourceProcessIter != topo->moduleProcessOrder.end());
    return sourceProcessIter < targetProcessIter;
  };

  return result;
}