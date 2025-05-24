#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>

#include <playground_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeEnvBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 4, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeEnvTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Env";
  result->slotCount = FFEnvCount;
  result->graphRenderer = FFEnvRenderGraph;
  result->id = "{FC1DC75A-200C-4465-8CBE-0100E2C8FAF2}";
  result->params.resize((int)FFEnvParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.env; });
  auto selectGuiModule = [](auto& state) { return &state.env; };
  auto selectModule = [](auto& state) { return &state.voice.env; };

  auto& on = result->params[(int)FFEnvParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{32C548EF-B46D-43D3-892F-51AE203212E6}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOn);
  on.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOn);
  on.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOn);

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
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);
  type.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& sync = result->params[(int)FFEnvParam::Sync];
  sync.acc = false;
  sync.name = "Sync";
  sync.slotCount = 1;
  sync.id = "{960C7FDA-5FA4-4719-9827-FCF94FCEEE99}";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSync);
  sync.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSync);
  sync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSync);
  sync.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& mode = result->params[(int)FFEnvParam::Mode];
  mode.acc = false;
  mode.defaultText = "Linear";
  mode.name = "Mode";
  mode.slotCount = 1;
  mode.id = "{F739A948-F9E6-4A22-9F56-52720704B74F}";
  mode.type = FBParamType::List;
  mode.List().items = {
    { "{59EB5AB9-50FC-4958-BABE-A126D65B7948}", "Linear" },
    { "{0B0F822E-A7D9-40B2-9B0B-7E404656DE3C}", "Exp" } };
  auto selectMode = [](auto& module) { return &module.block.mode; };
  mode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMode);
  mode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectMode);
  mode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMode);
  mode.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& delayTime = result->params[(int)FFEnvParam::DelayTime];
  delayTime.acc = false;
  delayTime.defaultText = "0";
  delayTime.display = "Dly";
  delayTime.name = "Delay Time";
  delayTime.slotCount = 1;
  delayTime.unit = "Sec";
  delayTime.id = "{D6A6CB86-A0D4-48A5-A495-038137E60519}";
  delayTime.type = FBParamType::Linear;
  delayTime.Linear().min = 0.0f;
  delayTime.Linear().max = 10.0f;
  delayTime.Linear().editSkewFactor = 0.5f;
  auto selectDelayTime = [](auto& module) { return &module.block.delayTime; };
  delayTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDelayTime);
  delayTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDelayTime);
  delayTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDelayTime);
  delayTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  delayTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& attackTime = result->params[(int)FFEnvParam::AttackTime];
  attackTime.acc = false;
  attackTime.defaultText = "0.03";
  attackTime.display = "Att";
  attackTime.name = "Attack Time";
  attackTime.slotCount = 1;
  attackTime.unit = "Sec";
  attackTime.id = "{193134E4-A104-419E-92A1-276E6CE1FA85}";
  attackTime.type = FBParamType::Linear;
  attackTime.Linear().min = 0.0f;
  attackTime.Linear().max = 10.0f;
  attackTime.Linear().editSkewFactor = 0.5f;
  auto selectAttackTime = [](auto& module) { return &module.block.attackTime; };
  attackTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAttackTime);
  attackTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAttackTime);
  attackTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAttackTime);
  attackTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  attackTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& holdTime = result->params[(int)FFEnvParam::HoldTime];
  holdTime.acc = false;
  holdTime.defaultText = "0";
  holdTime.display = "Hld";
  holdTime.name = "Hold Time";
  holdTime.slotCount = 1;
  holdTime.unit = "Sec";
  holdTime.id = "{29B5298F-C593-4E78-9D94-0FA1D36434B4}";
  holdTime.type = FBParamType::Linear;
  holdTime.Linear().min = 0.0f;
  holdTime.Linear().max = 10.0f;
  holdTime.Linear().editSkewFactor = 0.5f;
  auto selectHoldTime = [](auto& module) { return &module.block.holdTime; };
  holdTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectHoldTime);
  holdTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectHoldTime);
  holdTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectHoldTime);
  holdTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  holdTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& decayTime = result->params[(int)FFEnvParam::DecayTime];
  decayTime.acc = false;
  decayTime.defaultText = "0.1";
  decayTime.display = "Dcy";
  decayTime.name = "Decay Time";
  decayTime.slotCount = 1;
  decayTime.unit = "Sec";
  decayTime.id = "{DB9B7AB0-9FA2-4D6D-96D8-EA76D31B6F23}";
  decayTime.type = FBParamType::Linear;
  decayTime.Linear().min = 0.0f;
  decayTime.Linear().max = 10.0f;
  decayTime.Linear().editSkewFactor = 0.5f;
  auto selectDecayTime = [](auto& module) { return &module.block.decayTime; };
  decayTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDecayTime);
  decayTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDecayTime);
  decayTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDecayTime);
  decayTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  decayTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& releaseTime = result->params[(int)FFEnvParam::ReleaseTime];
  releaseTime.acc = false;
  releaseTime.defaultText = "0.2";
  releaseTime.display = "Rls";
  releaseTime.name = "Release Time";
  releaseTime.slotCount = 1;
  releaseTime.unit = "Sec";
  releaseTime.id = "{9AD9817D-295C-4911-BEF9-FEB46344BA8D}";
  releaseTime.type = FBParamType::Linear;
  releaseTime.Linear().min = 0.0f;
  releaseTime.Linear().max = 10.0f;
  releaseTime.Linear().editSkewFactor = 0.5f;
  auto selectReleaseTime = [](auto& module) { return &module.block.releaseTime; };
  releaseTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReleaseTime);
  releaseTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReleaseTime);
  releaseTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReleaseTime);
  releaseTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  releaseTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& smoothTime = result->params[(int)FFEnvParam::SmoothTime];
  smoothTime.acc = false;
  smoothTime.defaultText = "0";
  smoothTime.display = "Smth";
  smoothTime.name = "Smooth Time";
  smoothTime.slotCount = 1;
  smoothTime.unit = "Sec";
  smoothTime.id = "{D9B99AFC-8D45-4506-9D85-8978BF9BE317}";
  smoothTime.type = FBParamType::Linear;
  smoothTime.Linear().min = 0.0f;
  smoothTime.Linear().max = 10.0f;
  smoothTime.Linear().editSkewFactor = 0.5f;
  auto selectSmoothTime = [](auto& module) { return &module.block.smoothTime; };
  smoothTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSmoothTime);
  smoothTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSmoothTime);
  smoothTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSmoothTime);
  smoothTime.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  smoothTime.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& delayBars = result->params[(int)FFEnvParam::DelayBars];
  delayBars.acc = false;
  delayBars.defaultText = "Off";
  delayBars.display = "Dly";
  delayBars.name = "Delay Bars";
  delayBars.slotCount = 1;
  delayBars.unit = "Bars";
  delayBars.id = "{02BB4557-BFF4-4EBB-81FB-241861C94BDC}";
  delayBars.type = FBParamType::Bars;
  delayBars.Bars().items = MakeEnvBarsItems();
  auto selectDelayBars = [](auto& module) { return &module.block.delayBars; };
  delayBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDelayBars);
  delayBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDelayBars);
  delayBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDelayBars);
  delayBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  delayBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& attackBars = result->params[(int)FFEnvParam::AttackBars];
  attackBars.acc = false;
  attackBars.defaultText = "1/64";
  attackBars.display = "Att";
  attackBars.name = "Attack Bars";
  attackBars.slotCount = 1;
  attackBars.unit = "Bars";
  attackBars.id = "{F03A5681-CB9A-4E34-B73E-A4EB4EF8A2F4}";
  attackBars.type = FBParamType::Bars;
  attackBars.Bars().items = MakeEnvBarsItems();
  auto selectAttackBars = [](auto& module) { return &module.block.attackBars; };
  attackBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAttackBars);
  attackBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectAttackBars);
  attackBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAttackBars);
  attackBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  attackBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& holdBars = result->params[(int)FFEnvParam::HoldBars];
  holdBars.acc = false;
  holdBars.defaultText = "Off";
  holdBars.display = "Hld";
  holdBars.name = "Hold Bars";
  holdBars.slotCount = 1;
  holdBars.unit = "Bars";
  holdBars.id = "{67B31CCE-5B2D-4DFC-B4E6-A5027BB49E32}";
  holdBars.type = FBParamType::Bars;
  holdBars.Bars().items = MakeEnvBarsItems();
  auto selectHoldBars = [](auto& module) { return &module.block.holdBars; };
  holdBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectHoldBars);
  holdBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectHoldBars);
  holdBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectHoldBars);
  holdBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  holdBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& decayBars = result->params[(int)FFEnvParam::DecayBars];
  decayBars.acc = false;
  decayBars.defaultText = "1/32";
  decayBars.display = "Dcy";
  decayBars.name = "Decay Bars";
  decayBars.slotCount = 1;
  decayBars.unit = "Bars";
  decayBars.id = "{CD5B172C-FFA6-4F35-8AC7-E630B1F0A553}";
  decayBars.type = FBParamType::Bars;
  decayBars.Bars().items = MakeEnvBarsItems();
  auto selectDecayBars = [](auto& module) { return &module.block.decayBars; };
  decayBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDecayBars);
  decayBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDecayBars);
  decayBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDecayBars);
  decayBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  decayBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& releaseBars = result->params[(int)FFEnvParam::ReleaseBars];
  releaseBars.acc = false;
  releaseBars.defaultText = "1/16";
  releaseBars.display = "Rls";
  releaseBars.name = "Release Bars";
  releaseBars.slotCount = 1;
  releaseBars.unit = "Bars";
  releaseBars.id = "{1AFD7B11-2DCA-49F6-BD0A-4D1A4FBFD111}";
  releaseBars.type = FBParamType::Bars;
  releaseBars.Bars().items = MakeEnvBarsItems();
  auto selectReleaseBars = [](auto& module) { return &module.block.releaseBars; };
  releaseBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReleaseBars);
  releaseBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReleaseBars);
  releaseBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReleaseBars);
  releaseBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  releaseBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& smoothBars = result->params[(int)FFEnvParam::SmoothBars];
  smoothBars.acc = false;
  smoothBars.defaultText = "Off";
  smoothBars.display = "Smth";
  smoothBars.name = "Smooth Bars";
  smoothBars.slotCount = 1;
  smoothBars.unit = "Bars";
  smoothBars.id = "{8D2DEE38-2EA3-4FE3-BB77-D71847FC8666}";
  smoothBars.type = FBParamType::Bars;
  smoothBars.Bars().items = MakeEnvBarsItems();
  auto selectSmoothBars = [](auto& module) { return &module.block.smoothBars; };
  smoothBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSmoothBars);
  smoothBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSmoothBars);
  smoothBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSmoothBars);
  smoothBars.dependencies.visible.audio.When({ (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  smoothBars.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& attackSlope = result->params[(int)FFEnvParam::AttackSlope];
  attackSlope.acc = true;
  attackSlope.defaultText = "50";
  attackSlope.display = "ASlope";
  attackSlope.name = "Attack Slope";
  attackSlope.slotCount = 1;
  attackSlope.unit = "%";
  attackSlope.id = "{0C77104F-17CC-4256-8D65-FAD17E821758}";
  attackSlope.type = FBParamType::Identity;
  auto selectAttackSlope = [](auto& module) { return &module.acc.attackSlope; };
  attackSlope.scalarAddr = FFSelectScalarParamAddr(selectModule, selectAttackSlope);
  attackSlope.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectAttackSlope);
  attackSlope.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectAttackSlope);
  attackSlope.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Mode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEnvMode::Exp; });

  auto& decaySlope = result->params[(int)FFEnvParam::DecaySlope];
  decaySlope.acc = true;
  decaySlope.defaultText = "50";
  decaySlope.display = "DSlope";
  decaySlope.name = "Decay Slope";
  decaySlope.slotCount = 1;
  decaySlope.unit = "%";
  decaySlope.id = "{2F01EA4E-2665-4882-923D-FEF63D790F7B}";
  decaySlope.type = FBParamType::Identity;
  auto selectDecaySlope = [](auto& module) { return &module.acc.decaySlope; };
  decaySlope.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDecaySlope);
  decaySlope.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDecaySlope);
  decaySlope.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDecaySlope);
  decaySlope.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Mode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEnvMode::Exp; });

  auto& releaseSlope = result->params[(int)FFEnvParam::ReleaseSlope];
  releaseSlope.acc = true;
  releaseSlope.defaultText = "50";
  releaseSlope.display = "RSlope";
  releaseSlope.name = "Release Slope";
  releaseSlope.slotCount = 1;
  releaseSlope.unit = "%";
  releaseSlope.id = "{78A38181-41F7-4C0C-8489-F9AD55D6F2D9}";
  releaseSlope.type = FBParamType::Identity;
  auto selectReleaseSlope = [](auto& module) { return &module.acc.releaseSlope; };
  releaseSlope.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReleaseSlope);
  releaseSlope.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectReleaseSlope);
  releaseSlope.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReleaseSlope);
  releaseSlope.dependencies.enabled.audio.When({ (int)FFEnvParam::On, (int)FFEnvParam::Mode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEnvMode::Exp; });

  auto& sustainLevel = result->params[(int)FFEnvParam::SustainLevel];
  sustainLevel.acc = true;
  sustainLevel.defaultText = "50";
  sustainLevel.display = "Sustain";
  sustainLevel.name = "Sustain Level";
  sustainLevel.slotCount = 1;
  sustainLevel.unit = "%";
  sustainLevel.id = "{3B686952-A0CE-401D-97BC-20D159ADCF1C}";
  sustainLevel.type = FBParamType::Identity;
  auto selectSustainLevel = [](auto& module) { return &module.acc.sustainLevel; };
  sustainLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSustainLevel);
  sustainLevel.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectSustainLevel);
  sustainLevel.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSustainLevel);
  sustainLevel.dependencies.enabled.audio.When({ (int)FFEnvParam::On }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}