#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>
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
  auto moduleName = FBMakeRuntimeShortName(topo, module.tabName, module.slotCount, indices.module.slot, {}, false);
  return moduleName + " " + paramName;
}

std::unique_ptr<FBStaticModule>
FFMakeModMatrixTopo(bool global, FBStaticTopo const* topo)
{
  int slotCount = global ? FFModMatrixGlobalSlotCount : FFModMatrixVoiceSlotCount;
  std::string prefix = global ? "G" : "V";
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->name = global ? "Global Matrix" : "Voice Matrix";
  result->tabName = global ? "GMATRIX" : "VMATRIX";
  result->slotCount = 1;
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
  amount.defaultText = "0";
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
  auto selectSource = [](auto& module) { return &module.block.source; };
  source.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSource);
  source.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSource);
  source.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSource);
  source.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSource);
  source.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSource);
  source.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });
  
  int sourceIndex = 0;
  for (int m = 0; m < topo->modules.size(); m++)
  {
    auto const& module = topo->modules[m];
    if(!module.voice || !global)
      for (int ms = 0; ms < module.slotCount; ms++)
      {
        if (ms == 0)
          source.List().submenuStart[sourceIndex] = module.tabName;
        for (int o = 0; o < module.cvOutputs.size(); o++)
        {
          auto const& cvOutput = module.cvOutputs[o];
          for (int os = 0; os < cvOutput.slotCount; os++)
          {
            FBCVOutputTopoIndices indices = { { m, ms }, { o, os } };
            auto id = FBMakeRuntimeId(module.id, ms, cvOutput.id, os);
            auto name = FBMakeRuntimeCVOutputName(*topo, module, cvOutput, indices);
            source.List().items.push_back({ id, name });
            sourceIndex++;
          }
        }
      }
  }

  auto& target = result->params[(int)FFModMatrixParam::Target];
  target.acc = false;
  target.name = "Target";
  target.display = "Target";
  target.slotCount = slotCount;
  target.id = prefix + "{DB2C381F-7CA5-49FA-83C1-93DFECF9F97C}";
  target.type = FBParamType::List;
  auto selectTarget = [](auto& module) { return &module.block.target; };
  target.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTarget);
  target.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTarget);
  target.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTarget);
  target.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTarget);
  target.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTarget);
  target.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });

  int targetIndex = 0;
  for (int m = 0; m < topo->modules.size(); m++)
  {
    auto const& module = topo->modules[m];
    if(global != module.voice)
      for (int ms = 0; ms < module.slotCount; ms++)
      {
        std::string subMenuName = module.tabName;
        if (module.slotCount != 1)
          subMenuName += " " + std::to_string(ms + 1);
        target.List().submenuStart[targetIndex] = subMenuName;
        for (int p = 0; p < module.params.size(); p++)
        {
          auto const& param = module.params[p];
          if (param.AutomationTiming() == FBAutomationTiming::PerSample)
            for (int ps = 0; ps < param.slotCount; ps++)
            {
              FBParamTopoIndices indices = { { m, ms }, { p, ps } };
              auto id = FBMakeRuntimeId(module.id, ms, param.id, ps);
              auto name = MakeRuntimeParamModName(*topo, module, param, indices);
              target.List().items.push_back({ id, name });
              targetIndex++;
            }
        }
      }
  }

  return result;
}