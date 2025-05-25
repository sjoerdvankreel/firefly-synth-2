#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_plug/modules/effect/FFEffectGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeEffectTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "FX";
  result->slotCount = FFVEffectCount;
  result->graphCount = 1;
  result->graphRenderer = FFEffectRenderGraph;
  result->id = "{E1B159FB-C595-4EB1-AAE3-38E54A6CABC3}";
  result->params.resize((int)FFEffectParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vEffect; });
  auto selectModule = [](auto& state) { return &state.voice.vEffect; };

  auto& type = result->params[(int)FFEffectParam::Type];
  type.acc = false;
  type.defaultText = "Off";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{CDA13B60-22EA-441C-A7DA-2C3264BE9A77}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{B10B5760-6EB9-4B04-8D97-86E6836F64B1}", "Off" },
    { "{7F741742-C78C-40AC-8797-85DB69696E02}", "Filter" },
    { "{39A87BEA-5B76-46EE-8AE4-9545B40D183E}", "Comb" },
    { "{82CEC718-0694-42EF-8C44-61D051982637}", "Shaper" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);

  // todo not linear
  auto& shaperGain = result->params[(int)FFEffectParam::ShaperGain];
  shaperGain.acc = true;
  shaperGain.defaultText = "100";
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.unit = "%";
  shaperGain.id = "{5E9C03AC-0BE1-4B0D-8FEA-4C42F5995BC9}";
  shaperGain.type = FBParamType::Identity;
  auto selectShaperGain = [](auto& module) { return &module.acc.shaperGain; };
  shaperGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectShaperGain);
  shaperGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectShaperGain);
  shaperGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectShaperGain);

  return result;
}