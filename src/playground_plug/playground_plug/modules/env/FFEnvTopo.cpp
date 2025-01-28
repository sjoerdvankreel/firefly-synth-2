#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

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
  auto selectModule = [](auto& state) { return &state.voice.env; };

  auto& on = result->params[(int)FFEnvParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{32C548EF-B46D-43D3-892F-51AE203212E6}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  on.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& type = result->params[(int)FFEnvParam::Type];
  type.acc = false;
  type.defaultText = "Sustain";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{36179051-025E-4F4C-BBD7-108159165128}";
  type.type = FBParamType::List;
  type.list.items = {
    { "{D779DC38-B3EF-47B2-BBB1-3A12DD8292B4}", "Sustain" },
    { "{06A7F943-E9F6-48CB-BB6A-96BA4CCC995D}", "Follow" },
    { "{B3E3277E-7DEC-4F8C-935F-165AC0023F15}", "Release" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectType);
  type.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectType);

  auto& delayTime = result->params[(int)FFEnvParam::DelayTime];
  delayTime.acc = false;
  delayTime.defaultText = "0";
  delayTime.name = "Dly";
  delayTime.slotCount = 1;
  delayTime.unit = "Sec";
  delayTime.id = "{D6A6CB86-A0D4-48A5-A495-038137E60519}";
  delayTime.type = FBParamType::Linear;
  delayTime.linear.min = 0.0f;
  delayTime.linear.max = 10.0f;
  auto selectDelayTime = [](auto& module) { return &module.block.delayTime; };
  delayTime.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectDelayTime);
  delayTime.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectDelayTime);

  auto& attackTime = result->params[(int)FFEnvParam::AttackTime];
  attackTime.acc = false;
  attackTime.defaultText = "0.03";
  attackTime.name = "Att";
  attackTime.slotCount = 1;
  attackTime.unit = "Sec";
  attackTime.id = "{193134E4-A104-419E-92A1-276E6CE1FA85}";
  attackTime.type = FBParamType::Linear;
  attackTime.linear.min = 0.0f;
  attackTime.linear.max = 10.0f;
  auto selectAttackTime = [](auto& module) { return &module.block.attackTime; };
  attackTime.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectAttackTime);
  attackTime.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectAttackTime);

  auto& attackSlope = result->params[(int)FFEnvParam::AttackSlope];
  attackSlope.acc = true;
  attackSlope.defaultText = "0";
  attackSlope.name = "ASlp";
  attackSlope.slotCount = 1;
  attackSlope.unit = "TODO";
  attackSlope.id = "{0C77104F-17CC-4256-8D65-FAD17E821758}";
  attackSlope.type = FBParamType::Linear;
  auto selectAttackSlope = [](auto& module) { return &module.acc.attackSlope; };
  attackSlope.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectAttackSlope);
  attackSlope.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectAttackSlope);

  auto& holdTime = result->params[(int)FFEnvParam::HoldTime];
  holdTime.acc = false;
  holdTime.defaultText = "0";
  holdTime.name = "Hld";
  holdTime.slotCount = 1;
  holdTime.unit = "Sec";
  holdTime.id = "{29B5298F-C593-4E78-9D94-0FA1D36434B4}";
  holdTime.type = FBParamType::Linear;
  holdTime.linear.min = 0.0f;
  holdTime.linear.max = 10.0f;
  auto selectHoldTime = [](auto& module) { return &module.block.holdTime; };
  holdTime.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectHoldTime);
  holdTime.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectHoldTime);

  auto& decayTime = result->params[(int)FFEnvParam::DecayTime];
  decayTime.acc = false;
  decayTime.defaultText = "0.1";
  decayTime.name = "Dcy";
  decayTime.slotCount = 1;
  decayTime.unit = "Sec";
  decayTime.id = "{DB9B7AB0-9FA2-4D6D-96D8-EA76D31B6F23}";
  decayTime.type = FBParamType::Linear;
  decayTime.linear.min = 0.0f;
  decayTime.linear.max = 10.0f;
  auto selectDecayTime = [](auto& module) { return &module.block.decayTime; };
  decayTime.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectDecayTime);
  decayTime.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectDecayTime);

  auto& decaySlope = result->params[(int)FFEnvParam::DecaySlope];
  decaySlope.acc = true;
  decaySlope.defaultText = "0";
  decaySlope.name = "DSlp";
  decaySlope.slotCount = 1;
  decaySlope.unit = "TODO";
  decaySlope.id = "{2F01EA4E-2665-4882-923D-FEF63D790F7B}";
  decaySlope.type = FBParamType::Linear;
  auto selectDecaySlope = [](auto& module) { return &module.acc.decaySlope; };
  decaySlope.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectDecaySlope);
  decaySlope.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectDecaySlope);

  auto& sustainLevel = result->params[(int)FFEnvParam::SustainLevel];
  sustainLevel.acc = true;
  sustainLevel.defaultText = "0.5";
  sustainLevel.name = "Stn";
  sustainLevel.slotCount = 1;
  sustainLevel.unit = "TODO";
  sustainLevel.id = "{3B686952-A0CE-401D-97BC-20D159ADCF1C}";
  sustainLevel.type = FBParamType::Linear;
  auto selectSustainLevel = [](auto& module) { return &module.acc.sustainLevel; };
  sustainLevel.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectSustainLevel);
  sustainLevel.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectSustainLevel);

  auto& releaseTime = result->params[(int)FFEnvParam::ReleaseTime];
  releaseTime.acc = false;
  releaseTime.defaultText = "0.2";
  releaseTime.name = "Rls";
  releaseTime.slotCount = 1;
  releaseTime.unit = "Sec";
  releaseTime.id = "{9AD9817D-295C-4911-BEF9-FEB46344BA8D}";
  releaseTime.type = FBParamType::Linear;
  releaseTime.linear.min = 0.0f;
  releaseTime.linear.max = 10.0f;
  auto selectReleaseTime = [](auto& module) { return &module.block.releaseTime; };
  releaseTime.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectReleaseTime);
  releaseTime.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectReleaseTime);

  auto& releaseSlope = result->params[(int)FFEnvParam::ReleaseSlope];
  releaseSlope.acc = true;
  releaseSlope.defaultText = "0";
  releaseSlope.name = "RSlp";
  releaseSlope.slotCount = 1;
  releaseSlope.unit = "TODO";
  releaseSlope.id = "{78A38181-41F7-4C0C-8489-F9AD55D6F2D9}";
  releaseSlope.type = FBParamType::Linear;
  auto selectReleaseSlope = [](auto& module) { return &module.acc.releaseSlope; };
  releaseSlope.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectReleaseSlope);
  releaseSlope.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectReleaseSlope);

  return result;
}