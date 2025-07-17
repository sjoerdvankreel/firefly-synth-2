#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeCVOutput.hpp>

std::unique_ptr<FBStaticModule>
FFMakeModMatrixTopo(bool global, FBStaticTopo const* topo)
{
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
  opType.slotCount = FFModMatrixSlotCount;
  opType.defaultText = "Off";
  opType.id = prefix + "{8D28D968-8585-4A4D-B636-F365C5873973}";
  opType.type = FBParamType::List;
  opType.List().items = {
    { "{8E7F2BE6-12B7-483E-8308-DD96F63C7743}", "Off" },
    { "{33CE627C-A02D-43C0-A533-257E4D03EA1E}", "Add" },
    { "{F01ABE4C-C22E-47F2-900E-7E913906A740}", "Mul" },
    { "{91B784D0-E47A-46DC-ACD8-15A502E68A9A}", "Stk" } };
  auto selectOpType = [](auto& module) { return &module.block.opType; };
  opType.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOpType);
  opType.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOpType);
  opType.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOpType);
  opType.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOpType);
  opType.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOpType);

  auto& bipolar = result->params[(int)FFModMatrixParam::Bipolar];
  bipolar.acc = false;
  bipolar.name = "Bipolar";
  bipolar.slotCount = FFModMatrixSlotCount;
  bipolar.defaultText = "Off";
  bipolar.id = prefix + "{1D86D50C-12DC-4DDF-A471-780921BE837E}";
  bipolar.type = FBParamType::Boolean;
  auto selectBipolar = [](auto& module) { return &module.block.bipolar; };
  bipolar.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectBipolar);
  bipolar.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectBipolar);
  bipolar.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectBipolar);
  bipolar.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectBipolar);
  bipolar.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectBipolar);
  bipolar.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });

  auto& amount = result->params[(int)FFModMatrixParam::Amount];
  amount.acc = true;
  amount.defaultText = "0";
  amount.name = "Amount";
  amount.display = "Amt";
  amount.slotCount = FFModMatrixSlotCount;
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
  source.slotCount = FFModMatrixSlotCount;
  source.id = prefix + "{08DB9477-1B3A-4EC8-88C9-AF3A9ABA9CD8}";
  source.type = FBParamType::List;
  auto selectSource = [](auto& module) { return &module.block.source; };
  source.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSource);
  source.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSource);
  source.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSource);
  source.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSource);
  source.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSource);
  source.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });
  for (int m = 0; m < topo->modules.size(); m++)
  {
    auto const& module = topo->modules[m];
    for (int ms = 0; ms < module.slotCount; ms++)
      for (int o = 0; o < module.cvOutputs.size(); o++)
      {
        auto const& cvOutput = module.cvOutputs[o];
        for (int os = 0; os < cvOutput.slotCount; os++)
        {
          FBCVOutputTopoIndices indices = { { m, ms }, { o, os } };
          auto id = FBMakeRuntimeId(module.id, ms, cvOutput.id, os);
          auto name = MakeRuntimeCVOutputLongName(*topo, module, cvOutput, indices);
          source.List().items.push_back({ id, name });
        }
      }
  }

  auto& target = result->params[(int)FFModMatrixParam::Target];
  target.acc = false;
  target.name = "Target";
  target.slotCount = FFModMatrixSlotCount;
  target.id = prefix + "{DB2C381F-7CA5-49FA-83C1-93DFECF9F97C}";
  target.type = FBParamType::List;
  auto selectTarget = [](auto& module) { return &module.block.target; };
  target.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTarget);
  target.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTarget);
  target.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTarget);
  target.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTarget);
  target.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTarget);
  target.dependencies.enabled.audio.WhenSimple({ (int)FFModMatrixParam::OpType }, [](auto const& vs) { return vs[0] != 0; });
  for (int m = 0; m < topo->modules.size(); m++)
  {
    auto const& module = topo->modules[m];
    for (int ms = 0; ms < module.slotCount; ms++)
      for (int p = 0; p < module.params.size(); p++)
      {
        auto const& param = module.params[p];
        for (int ps = 0; ps < param.slotCount; ps++)
        {
          FBParamTopoIndices indices = { { m, ms }, { p, ps } };
          auto id = FBMakeRuntimeId(module.id, ms, param.id, ps);
          auto name = MakeRuntimeParamLongName(*topo, module, param, indices);
          target.List().items.push_back({ id, name });
        }
      }
  }

  return result;
}