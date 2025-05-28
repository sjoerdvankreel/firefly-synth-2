#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeEffectTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "FX";
  result->slotCount = FFEffectCount;
  result->graphCount = 1;
  result->id = "{154051CE-66D9-41C8-B479-C52D1111C962}";
  result->params.resize((int)FFEffectParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.effect; });
  auto selectModule = [](auto& state) { return &state.voice.effect; };

  auto& on = result->params[(int)FFEffectParam::On];
  on.acc = false;
  on.defaultText = "Off";
  on.name = "On";
  on.slotCount = 1;
  on.id = "{F8D1C8E9-AC09-40C0-90F7-18D2CA8F09F7}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOn);
  on.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOn);
  on.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& shaperGain = result->params[(int)FFEffectParam::ShaperGain];
  shaperGain.acc = true;
  shaperGain.defaultText = "100";
  shaperGain.name = "Shaper Gain";
  shaperGain.slotCount = 1;
  shaperGain.unit = "%";
  shaperGain.id = "{51D5655F-A8E0-47F9-B62D-67E4EFA24E4F}";
  shaperGain.type = FBParamType::Linear;
  shaperGain.Linear().min = 0.0f;
  shaperGain.Linear().max = 32.0f;
  shaperGain.Linear().displayMultiplier = 100;
  auto selectShaperGain = [](auto& module) { return &module.acc.shaperGain; };
  shaperGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectShaperGain);
  shaperGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectShaperGain);
  shaperGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectShaperGain);
  shaperGain.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}