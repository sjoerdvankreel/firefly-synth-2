#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeEnvTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Env";
  result->slotCount = FFEnvCount;
  result->renderGraph = FFEnvRenderGraph;
  result->id = "{FC1DC75A-200C-4465-8CBE-0100E2C8FAF2}";
  result->params.resize((int)FFEnvParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.env; });
  auto selectModule = [](auto& state) { return &state.voice.env; };

  auto& on = result->params[(int)FFEnvParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{32C548EF-B46D-43D3-892F-51AE203212E6}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& type = result->params[(int)FFEnvParam::Type];
  type.acc = false;
  type.defaultText = "Sustain";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{36179051-025E-4F4C-BBD7-108159165128}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{D779DC38-B3EF-47B2-BBB1-3A12DD8292B4}", "Sustain" },
    { "{06A7F943-E9F6-48CB-BB6A-96BA4CCC995D}", "Follow" },
    { "{B3E3277E-7DEC-4F8C-935F-165AC0023F15}", "Release" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectType);
  type.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectType);
  type.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectType);

  auto& sustainLevel = result->params[(int)FFEnvParam::SustainLevel];
  sustainLevel.acc = true;
  sustainLevel.defaultText = "50";
  sustainLevel.name = "Stn";
  sustainLevel.tooltip = "Sustain Level";
  sustainLevel.slotCount = 1;
  sustainLevel.unit = "%";
  sustainLevel.id = "{3B686952-A0CE-401D-97BC-20D159ADCF1C}";
  sustainLevel.type = FBParamType::Linear;
  sustainLevel.Linear().displayMultiplier = 100.0f;
  auto selectSustainLevel = [](auto& module) { return &module.acc.sustainLevel; };
  sustainLevel.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectSustainLevel);
  sustainLevel.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectSustainLevel);
  sustainLevel.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectSustainLevel);

  auto& sync = result->params[(int)FFEnvParam::Sync];
  sync.acc = false;
  sync.name = "Sync";
  sync.tooltip = "Tempo Sync";
  sync.slotCount = 1;
  sync.id = "{960C7FDA-5FA4-4719-9827-FCF94FCEEE99}";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectSync);
  sync.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectSync);
  sync.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectSync);

  auto& mode = result->params[(int)FFEnvParam::Mode];
  mode.acc = false;
  mode.defaultText = "Linear";
  mode.name = "Mode";
  mode.slotCount = 1;
  mode.id = "{F739A948-F9E6-4A22-9F56-52720704B74F}";
  mode.type = FBParamType::List;
  mode.List().items = {
    { "{59EB5AB9-50FC-4958-BABE-A126D65B7948}", "Linear" },
    { "{0B0F822E-A7D9-40B2-9B0B-7E404656DE3C}", "Exp", "Exponential" } };
  auto selectMode = [](auto& module) { return &module.block.mode; };
  mode.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectMode);
  mode.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectMode);
  mode.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectMode);

  auto& delayTime = result->params[(int)FFEnvParam::DelayTime];
  delayTime.acc = false;
  delayTime.defaultText = "0";
  delayTime.name = "Dly";
  delayTime.tooltip = "Delay Time";
  delayTime.slotCount = 1;
  delayTime.unit = "Sec";
  delayTime.id = "{D6A6CB86-A0D4-48A5-A495-038137E60519}";
  delayTime.type = FBParamType::Linear;
  delayTime.Linear().min = 0.0f;
  delayTime.Linear().max = 10.0f;
  auto selectDelayTime = [](auto& module) { return &module.block.delayTime; };
  delayTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDelayTime);
  delayTime.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDelayTime);
  delayTime.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDelayTime);
  delayTime.dependencies.visible.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });

  auto& attackTime = result->params[(int)FFEnvParam::AttackTime];
  attackTime.acc = false;
  attackTime.defaultText = "0.03";
  attackTime.name = "Att";
  attackTime.tooltip = "Attack Time";
  attackTime.slotCount = 1;
  attackTime.unit = "Sec";
  attackTime.id = "{193134E4-A104-419E-92A1-276E6CE1FA85}";
  attackTime.type = FBParamType::Linear;
  attackTime.Linear().min = 0.0f;
  attackTime.Linear().max = 10.0f;
  auto selectAttackTime = [](auto& module) { return &module.block.attackTime; };
  attackTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectAttackTime);
  attackTime.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectAttackTime);
  attackTime.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectAttackTime);

  auto& holdTime = result->params[(int)FFEnvParam::HoldTime];
  holdTime.acc = false;
  holdTime.defaultText = "0";
  holdTime.name = "Hld";
  holdTime.tooltip = "Hold Time";
  holdTime.slotCount = 1;
  holdTime.unit = "Sec";
  holdTime.id = "{29B5298F-C593-4E78-9D94-0FA1D36434B4}";
  holdTime.type = FBParamType::Linear;
  holdTime.Linear().min = 0.0f;
  holdTime.Linear().max = 10.0f;
  auto selectHoldTime = [](auto& module) { return &module.block.holdTime; };
  holdTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectHoldTime);
  holdTime.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectHoldTime);
  holdTime.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectHoldTime);

  auto& decayTime = result->params[(int)FFEnvParam::DecayTime];
  decayTime.acc = false;
  decayTime.defaultText = "0.1";
  decayTime.name = "Dcy";
  decayTime.tooltip = "Decay Time";
  decayTime.slotCount = 1;
  decayTime.unit = "Sec";
  decayTime.id = "{DB9B7AB0-9FA2-4D6D-96D8-EA76D31B6F23}";
  decayTime.type = FBParamType::Linear;
  decayTime.Linear().min = 0.0f;
  decayTime.Linear().max = 10.0f;
  auto selectDecayTime = [](auto& module) { return &module.block.decayTime; };
  decayTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDecayTime);
  decayTime.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDecayTime);
  decayTime.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDecayTime);

  auto& releaseTime = result->params[(int)FFEnvParam::ReleaseTime];
  releaseTime.acc = false;
  releaseTime.defaultText = "0.2";
  releaseTime.name = "Rls";
  releaseTime.tooltip = "Release Time";
  releaseTime.slotCount = 1;
  releaseTime.unit = "Sec";
  releaseTime.id = "{9AD9817D-295C-4911-BEF9-FEB46344BA8D}";
  releaseTime.type = FBParamType::Linear;
  releaseTime.Linear().min = 0.0f;
  releaseTime.Linear().max = 10.0f;
  auto selectReleaseTime = [](auto& module) { return &module.block.releaseTime; };
  releaseTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectReleaseTime);
  releaseTime.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectReleaseTime);
  releaseTime.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectReleaseTime);

  auto& smoothTime = result->params[(int)FFEnvParam::SmoothTime];
  smoothTime.acc = false;
  smoothTime.defaultText = "0";
  smoothTime.name = "Smth";
  smoothTime.tooltip = "Smoothing Time";
  smoothTime.slotCount = 1;
  smoothTime.unit = "Ms";
  smoothTime.id = "{D9B99AFC-8D45-4506-9D85-8978BF9BE317}";
  smoothTime.type = FBParamType::Linear;
  smoothTime.Linear().min = 0.0f;
  smoothTime.Linear().max = 1.0f;
  smoothTime.Linear().displayMultiplier = 1000.0f;
  auto selectSmoothTime = [](auto& module) { return &module.block.smoothTime; };
  smoothTime.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectSmoothTime);
  smoothTime.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectSmoothTime);
  smoothTime.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectSmoothTime);

  auto& delayBars = result->params[(int)FFEnvParam::DelayBars];
  delayBars.acc = false;
  delayBars.defaultText = "1/4";
  delayBars.name = "Dly";
  delayBars.tooltip = "Delay Bars";
  delayBars.slotCount = 1;
  delayBars.unit = "Bars";
  delayBars.id = "{02BB4557-BFF4-4EBB-81FB-241861C94BDC}";
  delayBars.type = FBParamType::TimeSig;
  delayBars.TimeSig().items = FBMakeTimeSigItems({ 1, 128 }, { 4, 1 });
  auto selectDelayBars = [](auto& module) { return &module.block.delayBars; };
  delayBars.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDelayBars);
  delayBars.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectDelayBars);
  delayBars.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDelayBars);
  delayBars.dependencies.visible.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });

  auto& attackSlope = result->params[(int)FFEnvParam::AttackSlope];
  attackSlope.acc = true;
  attackSlope.defaultText = "50";
  attackSlope.name = "ASlp";
  attackSlope.tooltip = "Attack Slope";
  attackSlope.slotCount = 1;
  attackSlope.unit = "%";
  attackSlope.id = "{0C77104F-17CC-4256-8D65-FAD17E821758}";
  attackSlope.type = FBParamType::Linear;
  attackSlope.Linear().displayMultiplier = 100.0f;
  auto selectAttackSlope = [](auto& module) { return &module.acc.attackSlope; };
  attackSlope.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectAttackSlope);
  attackSlope.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectAttackSlope);
  attackSlope.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectAttackSlope);
  attackSlope.dependencies.enabled.When({ (int)FFEnvParam::Mode }, [](auto const& vs) { return vs[0] == (int)FFEnvMode::Exp; });

  auto& decaySlope = result->params[(int)FFEnvParam::DecaySlope];
  decaySlope.acc = true;
  decaySlope.defaultText = "50";
  decaySlope.name = "DSlp";
  decaySlope.tooltip = "Decay Slope";
  decaySlope.slotCount = 1;
  decaySlope.unit = "%";
  decaySlope.id = "{2F01EA4E-2665-4882-923D-FEF63D790F7B}";
  decaySlope.type = FBParamType::Linear;
  decaySlope.Linear().displayMultiplier = 100.0f;
  auto selectDecaySlope = [](auto& module) { return &module.acc.decaySlope; };
  decaySlope.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectDecaySlope);
  decaySlope.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectDecaySlope);
  decaySlope.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectDecaySlope);
  decaySlope.dependencies.enabled.When({ (int)FFEnvParam::Mode }, [](auto const& vs) { return vs[0] == (int)FFEnvMode::Exp; });

  auto& releaseSlope = result->params[(int)FFEnvParam::ReleaseSlope];
  releaseSlope.acc = true;
  releaseSlope.defaultText = "50";
  releaseSlope.name = "RSlp";
  releaseSlope.tooltip = "Release Slope";
  releaseSlope.slotCount = 1;
  releaseSlope.unit = "%";
  releaseSlope.id = "{78A38181-41F7-4C0C-8489-F9AD55D6F2D9}";
  releaseSlope.type = FBParamType::Linear;
  releaseSlope.Linear().displayMultiplier = 100.0f;
  auto selectReleaseSlope = [](auto& module) { return &module.acc.releaseSlope; };
  releaseSlope.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectReleaseSlope);
  releaseSlope.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectReleaseSlope);
  releaseSlope.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectReleaseSlope);
  releaseSlope.dependencies.enabled.When({ (int)FFEnvParam::Mode }, [](auto const& vs) { return vs[0] == (int)FFEnvMode::Exp; });

  return result;
}