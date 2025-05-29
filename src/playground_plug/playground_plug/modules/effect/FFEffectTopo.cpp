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
  type.name = "Type";
  type.slotCount = FFEffectBlockCount;
  type.id = "{1585A19D-639E-4202-B60B-BD8560BC8B70}";
  type.defaultTextSelector = [](int ms, int ps) { return ps == 0 ? "StVar" : "Off"; };
  type.type = FBParamType::List;
  type.List().items = {
    { "{46A4BE9B-1072-4811-B3A1-3A463D0BA534}", "Off" },
    { "{348FED12-9753-4C48-9D21-BB8D21E036AB}", "StVar" },
    { "{B74E9485-459E-4017-ACF4-8466FBBF51EF}", "Comb" },
    { "{3DA2A1FC-6683-4F38-9443-18D9CBB7A684}", "Dist" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);
  type.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& stVarMode = result->params[(int)FFEffectParam::StVarMode];
  stVarMode.acc = false;
  stVarMode.defaultText = "LPF";
  stVarMode.name = "StVar Mode";
  stVarMode.display = "Mode";
  stVarMode.slotCount = FFEffectBlockCount;
  stVarMode.id = "{275B2C8D-6D21-4741-AB69-D21FA95CD7F5}";
  stVarMode.type = FBParamType::List;
  stVarMode.List().items = {
    { "{EAAE7102-9F6C-4EC2-8C39-B13BBDFF7AD1}", "LPF" },
    { "{6A91C381-DB9F-4CAA-8155-4A407700661A}", "BPF" },
    { "{747DA91C-C43D-4CFC-8EFD-353B0AC23E0E}", "HPF" },
    { "{10FEE670-AB90-4DBF-A617-6F15F3E4602D}", "BSF" },
    { "{EE9A4F79-B557-43B4-ABDF-320414D773D5}", "APF" },
    { "{C9C3A3F5-5C5B-4331-8F6E-DDD2DC5A1D7B}", "PEQ" },
    { "{8D885EEB-DF69-455A-9FFD-BA95E3E30596}", "BLL" },
    { "{C4BBC616-CFDB-4E93-9315-D25552D85F71}", "LSH" },
    { "{AF2550ED-90C5-4E09-996D-A4669728C744}", "HSH" } };
  auto selectStVarMode = [](auto& module) { return &module.block.stVarMode; };
  stVarMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStVarMode);
  stVarMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectStVarMode);
  stVarMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStVarMode);
  stVarMode.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectType::StVar; });
  stVarMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::StVar; });

  auto& stVarKeyTrk = result->params[(int)FFEffectParam::StVarKeyTrak];
  stVarKeyTrk.acc = true;
  stVarKeyTrk.defaultText = "0";
  stVarKeyTrk.name = "StVar KeyTrk";
  stVarKeyTrk.display = "KTrk";
  stVarKeyTrk.slotCount = FFEffectBlockCount;
  stVarKeyTrk.unit = "%";
  stVarKeyTrk.id = "{CC91F0B0-9D53-4140-B698-0561D04F500C}";
  stVarKeyTrk.type = FBParamType::Linear;
  stVarKeyTrk.Linear().min = -2.0f;
  stVarKeyTrk.Linear().max = 2.0f;
  stVarKeyTrk.Linear().displayMultiplier = 100;
  auto selectStVarKeyTrk = [](auto& module) { return &module.acc.stVarKeyTrk; };
  stVarKeyTrk.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStVarKeyTrk);
  stVarKeyTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStVarKeyTrk);
  stVarKeyTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStVarKeyTrk);
  stVarKeyTrk.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectType::StVar; });
  stVarKeyTrk.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::StVar; });

  auto& stVarRes = result->params[(int)FFEffectParam::StVarRes];
  stVarRes.acc = true;
  stVarRes.defaultText = "0";
  stVarRes.name = "StVar Res";
  stVarRes.display = "Res";
  stVarRes.slotCount = FFEffectBlockCount;
  stVarRes.unit = "%";
  stVarRes.id = "{0B7CCB1C-FF95-46CD-9C93-1BAF5CE350E3}";
  stVarRes.type = FBParamType::Identity;
  auto selectStVarRes = [](auto& module) { return &module.acc.stVarRes; };
  stVarRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStVarRes);
  stVarRes.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStVarRes);
  stVarRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStVarRes);
  stVarRes.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectType::StVar; });
  stVarRes.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::StVar; });

  auto& stVarFreq = result->params[(int)FFEffectParam::StVarFreq];
  stVarFreq.acc = true;
  stVarFreq.defaultText = "1000";
  stVarFreq.name = "StVar Freq";
  stVarFreq.display = "Frq";
  stVarFreq.slotCount = FFEffectBlockCount;
  stVarFreq.unit = "Hz";
  stVarFreq.id = "{4ACF9D3E-D1F0-4B3B-8F6F-6FEE6BCDE449}";
  stVarFreq.type = FBParamType::Log2;
  stVarFreq.Log2().Init(0.0f, 20.0f, 20000.0f);
  auto selectStVarFreq = [](auto& module) { return &module.acc.stVarFreq; };
  stVarFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStVarFreq);
  stVarFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStVarFreq);
  stVarFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStVarFreq);
  stVarFreq.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectType::StVar; });
  stVarFreq.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::StVar; });

  auto& stVarGain = result->params[(int)FFEffectParam::StVarGain];
  stVarGain.acc = true;
  stVarGain.defaultText = "0";
  stVarGain.name = "StVar Gain";
  stVarGain.display = "Gn";
  stVarGain.slotCount = FFEffectBlockCount;
  stVarGain.unit = "dB";
  stVarGain.id = "{CA06747B-F867-4E03-AF36-327662021440}";
  stVarGain.type = FBParamType::Linear;
  stVarGain.Linear().min = -24.0f;
  stVarGain.Linear().max = 24.0f;
  auto selectStVarGain = [](auto& module) { return &module.acc.stVarGain; };
  stVarGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStVarGain);
  stVarGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStVarGain);
  stVarGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStVarGain);
  stVarGain.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectType::StVar; });
  stVarGain.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type, (int)FFEffectParam::StVarMode}, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::StVar && vs[2] >= (int)FBCytomicFilterMode::BLL; });

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

  auto& distMode = result->params[(int)FFEffectParam::DistMode];
  distMode.acc = false;
  distMode.defaultText = "Clip";
  distMode.name = "Dist Mode";
  distMode.display = "Mode";
  distMode.slotCount = FFEffectBlockCount;
  distMode.id = "{317BA4AC-8E9A-47B9-A289-294047E29C78}";
  distMode.type = FBParamType::List;
  distMode.List().items = {
    { "{89B7DFF2-E7D6-4D55-8060-C7BD00734AC2}", "Clip" },
    { "{CF8ED573-1755-40BC-B7BF-BD2F330E795F}", "Shape" },
    { "{F73CA415-19C6-4E53-BC86-B028DC64370C}", "Skew" } };
  auto selectDistMode = [](auto& module) { return &module.block.distMode; };
  distMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistMode);
  distMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDistMode);
  distMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistMode);
  distMode.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist; });
  distMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist; });

  auto& distGain = result->params[(int)FFEffectParam::DistGain];
  distGain.acc = true;
  distGain.defaultText = "100";
  distGain.name = "Dist Gain";
  distGain.display = "Gn";
  distGain.slotCount = FFEffectBlockCount;
  distGain.unit = "%";
  distGain.id = "{971B9F5B-0348-4F56-A6A0-DC40FC4B32BD}";
  distGain.type = FBParamType::Linear;
  distGain.Linear().min = 0.0f;
  distGain.Linear().max = 32.0f;
  distGain.Linear().displayMultiplier = 100;
  auto selectDistGain = [](auto& module) { return &module.acc.distGain; };
  distGain.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistGain);
  distGain.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistGain);
  distGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistGain);
  distGain.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist; });
  distGain.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist; });

  auto& distMix = result->params[(int)FFEffectParam::DistMix];
  distMix.acc = true;
  distMix.defaultText = "100";
  distMix.name = "Dist Mix";
  distMix.display = "Mix";
  distMix.slotCount = FFEffectBlockCount;
  distMix.id = "{CD542E15-A8DD-4A72-9B75-E8D8301D8F05}";
  distMix.type = FBParamType::Identity;
  auto selectDistMix = [](auto& module) { return &module.acc.distMix; };
  distMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistMix);
  distMix.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistMix);
  distMix.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistMix);
  distMix.dependencies.visible.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist; });
  distMix.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist; });

  auto& distClipMode = result->params[(int)FFEffectParam::DistClipMode];
  distClipMode.acc = false;
  distClipMode.defaultText = "Hard";
  distClipMode.name = "Dist Clip Mode";
  distClipMode.display = "Mode";
  distClipMode.slotCount = FFEffectBlockCount;
  distClipMode.id = "{D1F80BB8-4076-4296-A678-94E8442C51A5}";
  distClipMode.type = FBParamType::List;
  distClipMode.List().items = {
    { "{32F53B15-54AC-44AE-8812-97D598B9928B}", "Hard" },
    { "{E4ECBDA0-F14F-411D-81A8-C59CC9B7C2C6}", "TanH" },
    { "{851F55D9-89E0-4B37-A6DA-A81692A716BD}", "Sin" },
    { "{A629BC3E-4732-4A6A-AB79-38CE84F04B0D}", "TSQ" },
    { "{C0E30CBB-596C-4267-96E9-9FBFD5D26C27}", "Cube" },
    { "{192BF63E-663D-494C-956F-3A8BB2E22067}", "Inv" },
    { "{8A58AAB2-0AE7-426E-B71F-A444653286A6}", "Exp" } };
  auto selectDistClipMode = [](auto& module) { return &module.block.distClipMode; };
  distClipMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistClipMode);
  distClipMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDistClipMode);
  distClipMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistClipMode);
  distClipMode.dependencies.visible.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist && vs[1] == (int)FFEffectDistMode::Clip; });
  distClipMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist && vs[2] == (int)FFEffectDistMode::Clip; });

  auto& distClipExp = result->params[(int)FFEffectParam::DistClipExp];
  distClipExp.acc = true;
  distClipExp.defaultText = "1";
  distClipExp.name = "Dist Clip Exp";
  distClipExp.display = "Exp";
  distClipExp.slotCount = FFEffectBlockCount;
  distClipExp.id = "{A5901BBF-3B8A-433A-99F5-10DF84ADB1F8}";
  distClipExp.type = FBParamType::Linear;
  distClipExp.Linear().min = 0.1f;
  distClipExp.Linear().max = 10.0f;
  auto selectDistClipExp = [](auto& module) { return &module.acc.distClipExp; };
  distClipExp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistClipExp);
  distClipExp.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistClipExp);
  distClipExp.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistClipExp);
  distClipExp.dependencies.visible.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist && vs[1] == (int)FFEffectDistMode::Clip; });
  distClipExp.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type, (int)FFEffectParam::DistMode, (int)FFEffectParam::DistClipExp }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist && vs[2] == (int)FFEffectDistMode::Clip && vs[3] == (int)FFEffectDistClipMode::Exp; });

  auto& distSkewMode = result->params[(int)FFEffectParam::DistSkewMode];
  distSkewMode.acc = false;
  distSkewMode.defaultText = "Uni";
  distSkewMode.name = "Dist Skew Mode";
  distSkewMode.display = "Mode";
  distSkewMode.slotCount = FFEffectBlockCount;
  distSkewMode.id = "{02008903-8467-49DF-BC28-B435BAFD1B6A}";
  distSkewMode.type = FBParamType::List;
  distSkewMode.List().items = {
    { "{ABE6AB1B-A19B-4338-B9E8-2976DDD8AFC2}", "Uni" },
    { "{ABE6AB1B-A19B-4338-B9E8-2976DDD8AFC2}", "Bi" } };
  auto selectDistSkewMode = [](auto& module) { return &module.block.distSkewMode; };
  distSkewMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistSkewMode);
  distSkewMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectDistSkewMode);
  distSkewMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistSkewMode);
  distSkewMode.dependencies.visible.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist && vs[1] == (int)FFEffectDistMode::Skew; });
  distSkewMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist && vs[2] == (int)FFEffectDistMode::Skew; });
  
  auto& distSkewAmt = result->params[(int)FFEffectParam::DistSkewAmt];
  distSkewAmt.acc = true;
  distSkewAmt.defaultText = "50";
  distSkewAmt.name = "Dist Skew Amt";
  distSkewAmt.display = "Amt";
  distSkewAmt.slotCount = FFEffectBlockCount;
  distSkewAmt.id = "{C78B596F-8059-44F0-B73D-A699AB647F54}";
  distSkewAmt.type = FBParamType::Identity;
  auto selectDistSkewAmt = [](auto& module) { return &module.acc.distSkewAmt; };
  distSkewAmt.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistSkewAmt);
  distSkewAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistSkewAmt);
  distSkewAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistSkewAmt);
  distSkewAmt.dependencies.visible.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] == (int)FFEffectType::Dist && vs[1] == (int)FFEffectDistMode::Skew; });
  distSkewAmt.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Type, (int)FFEffectParam::DistMode }, [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectType::Dist && vs[2] == (int)FFEffectDistMode::Skew; });

  return result;
}