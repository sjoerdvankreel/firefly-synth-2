#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeModMatrixTopo(bool global)
{
  std::string prefix = global ? "G" : "V";
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->name = global ? "Global Matrix" : "Voice Matrix";
  result->tabName = global ? "GMatrix" : "VMatrix";
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

  return result;
}