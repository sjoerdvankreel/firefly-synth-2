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
  oversample.name = "Oversmp";
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

  auto& combKeyTrk = result->params[(int)FFEffectParam::CombKeyTrk];
  combKeyTrk.acc = true;
  combKeyTrk.defaultText = "0";
  combKeyTrk.name = "Comb KeyTrk";
  combKeyTrk.display = "KTrk";
  combKeyTrk.slotCount = FFEffectBlockCount;
  combKeyTrk.unit = "%";
  combKeyTrk.id = "{77B1716F-4511-492B-A32E-F04CF668238B}";
  combKeyTrk.type = FBParamType::Linear;
  combKeyTrk.Linear().min = -2.0f;
  combKeyTrk.Linear().max = 2.0f;
  combKeyTrk.Linear().displayMultiplier = 100;
  auto selectCombKeyTrk = [](auto& module) { return &module.acc.combKeyTrk; };
  combKeyTrk.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombKeyTrk);
  combKeyTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombKeyTrk);
  combKeyTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombKeyTrk);
  combKeyTrk.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combKeyTrk.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });
  
  auto& combFreqPlus = result->params[(int)FFEffectParam::CombFreqPlus];
  combFreqPlus.acc = true;
  combFreqPlus.defaultText = "1000";
  combFreqPlus.name = "Comb Freq+";
  combFreqPlus.display = "Frq+";
  combFreqPlus.slotCount = FFEffectBlockCount;
  combFreqPlus.unit = "Hz";
  combFreqPlus.id = "{19B8B573-C49D-4D34-8078-02B2A30F40E8}";
  combFreqPlus.type = FBParamType::Log2;
  combFreqPlus.Log2().Init(0.0f, 20.0f, 20000.0f);
  auto selectCombFreqPlus = [](auto& module) { return &module.acc.combFreqPlus; };
  combFreqPlus.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombFreqPlus);
  combFreqPlus.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombFreqPlus);
  combFreqPlus.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombFreqPlus);
  combFreqPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combFreqPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  auto& combFreqMin = result->params[(int)FFEffectParam::CombFreqMin];
  combFreqMin.acc = true;
  combFreqMin.defaultText = "1000";
  combFreqMin.name = "Comb Freq-";
  combFreqMin.display = "Frq-";
  combFreqMin.slotCount = FFEffectBlockCount;
  combFreqMin.unit = "Hz";
  combFreqMin.id = "{B2C0AEB8-F2C1-4553-961C-7B61021C8B70}";
  combFreqMin.type = FBParamType::Log2;
  combFreqMin.Log2().Init(0.0f, 20.0f, 20000.0f);
  auto selectCombFreqMin = [](auto& module) { return &module.acc.combFreqMin; };
  combFreqMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombFreqMin);
  combFreqMin.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombFreqMin);
  combFreqMin.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombFreqMin);
  combFreqMin.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combFreqMin.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  auto& combResPlus = result->params[(int)FFEffectParam::CombResPlus];
  combResPlus.acc = true;
  combResPlus.defaultText = "50";
  combResPlus.name = "Comb Rz+";
  combResPlus.display = "Res+";
  combResPlus.slotCount = FFEffectBlockCount;
  combResPlus.unit = "%";
  combResPlus.id = "{7DF779CB-794D-4419-9E7F-E68F3F3BCB57}";
  combResPlus.type = FBParamType::Linear;
  combResPlus.Linear().min = -1.0f;
  combResPlus.Linear().max = 1.0f;
  combResPlus.Linear().displayMultiplier = 100;
  auto selectCombResPlus = [](auto& module) { return &module.acc.combResPlus; };
  combResPlus.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombResPlus);
  combResPlus.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombResPlus);
  combResPlus.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombResPlus);
  combResPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combResPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  auto& combResMin = result->params[(int)FFEffectParam::CombResMin];
  combResMin.acc = true;
  combResMin.defaultText = "0";
  combResMin.name = "Comb Res-";
  combResMin.display = "Res-";
  combResMin.slotCount = FFEffectBlockCount;
  combResMin.unit = "%";
  combResMin.id = "{09588739-10E7-413E-9577-5A9BE8996A5D}";
  combResMin.type = FBParamType::Linear;
  combResMin.Linear().min = -1.0f;
  combResMin.Linear().max = 1.0f;
  combResMin.Linear().displayMultiplier = 100;
  auto selectCombResMin = [](auto& module) { return &module.acc.combResMin; };
  combResMin.scalarAddr = FFSelectScalarParamAddr(selectModule, selectCombResMin);
  combResMin.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectCombResMin);
  combResMin.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectCombResMin);
  combResMin.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Comb; });
  combResMin.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Comb; });

  return result;
}