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

  auto& oversample = result->params[(int)FFEffectParam::Oversample];
  oversample.acc = false;
  oversample.defaultText = "Off";
  oversample.name = "Oversample";
  oversample.slotCount = 1;
  oversample.id = "{28875DF7-255B-4190-80CE-D0A9ED20F263}";
  oversample.type = FBParamType::Boolean;
  auto selectOversample = [](auto& module) { return &module.block.oversample; };
  oversample.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOversample);
  oversample.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOversample);
  oversample.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOversample);
  oversample.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& type = result->params[(int)FFEffectParam::Type];
  type.acc = false;
  type.defaultText = "Off"; // todo
  type.name = "Type";
  type.slotCount = FFEffectBlockCount;
  type.id = "{1585A19D-639E-4202-B60B-BD8560BC8B70}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{46A4BE9B-1072-4811-B3A1-3A463D0BA534}", "Off" },
    { "{B74E9485-459E-4017-ACF4-8466FBBF51EF}", "Shaper" }, // todo shape/clip/skew
    { "{3DA2A1FC-6683-4F38-9443-18D9CBB7A684}", "Comb" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);
  type.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& shaperGain = result->params[(int)FFEffectParam::ShaperGain];
  shaperGain.acc = true;
  shaperGain.defaultText = "100";
  shaperGain.name = "Shaper Gain";
  shaperGain.display = "Gain";
  shaperGain.slotCount = FFEffectBlockCount;
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
  shaperGain.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != (int)FFEffectType::Comb; });
  shaperGain.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type}, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Shaper; });

  auto& combDelayPlus = result->params[(int)FFEffectParam::CombDelayPlus];
  combDelayPlus.acc = true;
  combDelayPlus.defaultText = "1";
  combDelayPlus.name = "Comb Delay+";
  combDelayPlus.display = "Delay+";
  combDelayPlus.slotCount = FFEffectBlockCount;
  combDelayPlus.unit = "Ms";
  combDelayPlus.id = "{19B8B573-C49D-4D34-8078-02B2A30F40E8}";
  combDelayPlus.type = FBParamType::Linear;
  combDelayPlus.Linear().min = 0.1f / 1000.0f;
  combDelayPlus.Linear().max = 5.0f / 1000.0f;
  combDelayPlus.Linear().displayMultiplier = 1000;
  auto selectCombDelayPlus = [](auto& module) { return &module.acc.combDelayPlus; };
  combDelayPlus.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombDelayPlus);
  combDelayPlus.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombDelayPlus);
  combDelayPlus.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombDelayPlus);
  combDelayPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combDelayPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  auto& combDelayMin = result->params[(int)FFEffectParam::CombDelayMin];
  combDelayMin.acc = true;
  combDelayMin.defaultText = "1";
  combDelayMin.name = "Comb Delay-";
  combDelayMin.display = "Delay-";
  combDelayMin.slotCount = FFEffectBlockCount;
  combDelayMin.unit = "Ms";
  combDelayMin.id = "{B2C0AEB8-F2C1-4553-961C-7B61021C8B70}";
  combDelayMin.type = FBParamType::Linear;
  combDelayMin.Linear().min = 0.1f / 1000.0f;
  combDelayMin.Linear().max = 5.0f / 1000.0f;
  combDelayMin.Linear().displayMultiplier = 1000;
  auto selectCombDelayMin = [](auto& module) { return &module.acc.combDelayMin; };
  combDelayMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombDelayMin);
  combDelayMin.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombDelayMin);
  combDelayMin.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombDelayMin);
  combDelayMin.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combDelayMin.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  auto& combGainPlus = result->params[(int)FFEffectParam::CombGainPlus];
  combGainPlus.acc = true;
  combGainPlus.defaultText = "1";
  combGainPlus.name = "Comb Gain+";
  combGainPlus.display = "Gain+";
  combGainPlus.slotCount = FFEffectBlockCount;
  combGainPlus.unit = "%";
  combGainPlus.id = "{7DF779CB-794D-4419-9E7F-E68F3F3BCB57}";
  combGainPlus.type = FBParamType::Linear;
  combGainPlus.Linear().min = -1.0f;
  combGainPlus.Linear().max = 1.0f;
  combGainPlus.Linear().displayMultiplier = 100;
  auto selectCombGainPlus = [](auto& module) { return &module.acc.combGainPlus; };
  combGainPlus.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombGainPlus);
  combGainPlus.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombGainPlus);
  combGainPlus.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombGainPlus);
  combGainPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combGainPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  auto& combGainMin = result->params[(int)FFEffectParam::CombGainMin];
  combGainMin.acc = true;
  combGainMin.defaultText = "1";
  combGainMin.name = "Comb Gain-";
  combGainMin.display = "Gain-";
  combGainMin.slotCount = FFEffectBlockCount;
  combGainMin.unit = "%";
  combGainMin.id = "{09588739-10E7-413E-9577-5A9BE8996A5D}";
  combGainMin.type = FBParamType::Linear;
  combGainMin.Linear().min = -1.0f;
  combGainMin.Linear().max = 1.0f;
  combGainMin.Linear().displayMultiplier = 100;
  auto selectCombGainMin = [](auto& module) { return &module.acc.combGainMin; };
  combGainMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombGainMin);
  combGainMin.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombGainMin);
  combGainMin.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombGainMin);
  combGainMin.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combGainMin.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  return result;
}