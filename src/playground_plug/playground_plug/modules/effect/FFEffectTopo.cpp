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
  result->slotCount = FFEffectCount;
  result->graphCount = FFEffectBlockCount;
  result->graphRenderer = FFEffectRenderGraph;
  result->id = "{154051CE-66D9-41C8-B479-C52D1111C962}";
  result->params.resize((int)FFEffectParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.effect; });
  auto selectModule = [](auto& state) { return &state.voice.effect; };

  auto& type = result->params[(int)FFEffectParam::Type];
  type.acc = false;
  type.name = "Type";
  type.slotCount = 1;
  type.defaultText = "Off";
  type.id = "{85CEFF97-A98D-4470-BA15-825608A0E954}";
  type.type = FBParamType::List;
  type.List().items = {
    { "{BED24339-5660-4B8F-9A01-F2C7886E0C9F}", "Off" },
    { "{369304F5-3571-42C0-AB0B-F369BA427713}", "On" },
    { "{8A0B5B14-949E-4AFF-B670-1EAC9F14FE1A}", "Fdbk" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFSelectScalarParamAddr(selectModule, selectType);
  type.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectType);
  type.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectType);

  auto& feedback = result->params[(int)FFEffectParam::Feedback];
  feedback.acc = true;
  feedback.defaultText = "0";
  feedback.name = "Feedback";
  feedback.display = "Fdbk";
  feedback.slotCount = 1;
  feedback.unit = "%";
  feedback.id = "{19017F12-FE20-4623-9AB9-105E7CC26C0B}";
  feedback.type = FBParamType::Identity;
  auto selectFeedback = [](auto& module) { return &module.acc.feedback; };
  feedback.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedback);
  feedback.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedback);
  feedback.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedback);
  feedback.dependencies.enabled.audio.When({ (int)FFEffectParam::Type },
    [](auto const& vs) { return vs[0] == (int)FFEffectType::Feedback; });

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
  oversample.dependencies.enabled.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& trackingKey = result->params[(int)FFEffectParam::TrackingKey];
  trackingKey.acc = true;
  trackingKey.defaultText = "0";
  trackingKey.name = "Tracking Key";
  trackingKey.display = "Key";
  trackingKey.slotCount = 1;
  trackingKey.unit = "Semitones";
  trackingKey.id = "{409B79DA-5F59-4C54-AA3D-3E7E84B1A303}";
  trackingKey.type = FBParamType::Linear;
  trackingKey.Linear().min = -64.0f;
  trackingKey.Linear().max = 64.0f;
  auto selectTrackingKey = [](auto& module) { return &module.acc.trackingKey; };
  trackingKey.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTrackingKey);
  trackingKey.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectTrackingKey);
  trackingKey.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTrackingKey);
  trackingKey.dependencies.enabled.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0; });

  auto& kind = result->params[(int)FFEffectParam::Kind];
  kind.acc = false;
  kind.name = "Kind";
  kind.slotCount = FFEffectBlockCount;
  kind.id = "{1585A19D-639E-4202-B60B-BD8560BC8B70}";
  kind.defaultTextSelector = [](int ms, int ps) { return ps == 0 ? "StVar" : "Off"; };
  kind.type = FBParamType::List;
  kind.List().items = {
    { "{46A4BE9B-1072-4811-B3A1-3A463D0BA534}", "Off" },
    { "{348FED12-9753-4C48-9D21-BB8D21E036AB}", "StVar" },
    { "{B74E9485-459E-4017-ACF4-8466FBBF51EF}", "Comb" },
    { "{FD072A47-EE67-4091-A687-7168B69A6E89}", "Clip" },
    { "{06334343-5264-489E-ADF9-20ADCEF983FC}", "Fold" },
    { "{3DA2A1FC-6683-4F38-9443-18D9CBB7A684}", "Skew" } };
  kind.List().submenuStart[(int)FFEffectKind::Off] = "Off";
  kind.List().submenuStart[(int)FFEffectKind::StVar] = "Filter";
  kind.List().submenuStart[(int)FFEffectKind::Clip] = "Shape";
  auto selectKind = [](auto& module) { return &module.block.kind; };
  kind.scalarAddr = FFSelectScalarParamAddr(selectModule, selectKind);
  kind.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectKind);
  kind.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectKind);
  kind.dependencies.enabled.audio.When({ (int)FFEffectParam::Type }, [](auto const& vs) { return vs[0] != 0; });

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
  stVarMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarMode.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

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
  stVarKeyTrk.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarKeyTrk.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

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
  stVarRes.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarRes.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

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
  stVarFreq.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarFreq.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

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
  stVarGain.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarGain.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind, (int)FFEffectParam::StVarMode},
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar && vs[2] >= (int)FBCytomicFilterMode::BLL; });

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
  combKeyTrk.dependencies.visible.audio.When({ (int)FFEffectParam::Kind }, 
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combKeyTrk.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });
  
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
  combFreqPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combFreqPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

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
  combFreqMin.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combFreqMin.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

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
  combResPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combResPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

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
  combResMin.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combResMin.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

  auto& clipMode = result->params[(int)FFEffectParam::ClipMode];
  clipMode.acc = false;
  clipMode.defaultText = "TanH";
  clipMode.name = "Clip Mode";
  clipMode.display = "Mode";
  clipMode.slotCount = FFEffectBlockCount;
  clipMode.id = "{D1F80BB8-4076-4296-A678-94E8442C51A5}";
  clipMode.type = FBParamType::List;
  clipMode.List().items = {
    { "{32F53B15-54AC-44AE-8812-97D598B9928B}", "Hard" },
    { "{E4ECBDA0-F14F-411D-81A8-C59CC9B7C2C6}", "TanH" },
    { "{851F55D9-89E0-4B37-A6DA-A81692A716BD}", "Sin" },
    { "{A629BC3E-4732-4A6A-AB79-38CE84F04B0D}", "TSQ" },
    { "{C0E30CBB-596C-4267-96E9-9FBFD5D26C27}", "Cube" },
    { "{192BF63E-663D-494C-956F-3A8BB2E22067}", "Inv" },
    { "{8A58AAB2-0AE7-426E-B71F-A444653286A6}", "Exp" } };
  auto selectClipMode = [](auto& module) { return &module.block.clipMode; };
  clipMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectClipMode);
  clipMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectClipMode);
  clipMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectClipMode);
  clipMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Clip; });
  clipMode.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Clip; });

  // todo submenu
  auto& foldMode = result->params[(int)FFEffectParam::FoldMode];
  foldMode.acc = false;
  foldMode.defaultText = "Sin";
  foldMode.name = "Fold Mode";
  foldMode.display = "Mode";
  foldMode.slotCount = FFEffectBlockCount;
  foldMode.id = "{317BA4AC-8E9A-47B9-A289-294047E29C78}";
  foldMode.type = FBParamType::List;
  foldMode.List().items = {
    { "{55167ED0-D050-403B-A061-CA4D0916E400}", "Fold" },
    { "{129369F6-C303-4BBA-8573-06FC33972FD9}", "Sin" },
    { "{8CFCDA01-C9A9-4231-9994-8480CC08A1CE}", "Cos" },
    { "{549CC93F-C88A-4C3B-AD37-B3C818DFF573}", "Sin2" },
    { "{544151DB-5403-4732-B416-6CA2C9C78066}", "Cos2" },
    { "{C10D2D9B-53F1-4779-AE80-D463A0DD7278}", "SnCs" },
    { "{D820E334-2D19-4306-914D-00AA7D048D48}", "CsSn" },
    { "{BB19624C-893C-41F9-83AD-CB17DFD9FC60}", "Sin3" },
    { "{66737A7F-6548-4881-B8F3-69FCF0EB1843}", "Cos3" },
    { "{2133F2BE-95B6-4845-A622-5712F2747960}", "S2Cs" },
    { "{5C572DE3-6197-4289-A009-573A88E2B09F}", "C2Sn" },
    { "{7527549D-68FE-4D6F-B420-BA75F9097EEE}", "SnC2" },
    { "{7DA4D108-2DCB-49C1-97D3-A3528A3BD715}", "CsS2" },
    { "{EAEFCA78-2779-484D-AC67-CD61786B64B5}", "SCS" },
    { "{4C0E5578-38F2-411C-A266-8FD9FFEA8612}", "CSC" } };
  auto selectFoldMode = [](auto& module) { return &module.block.foldMode; };
  foldMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFoldMode);
  foldMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFoldMode);
  foldMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFoldMode);
  foldMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Fold; });
  foldMode.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Fold; });
  
  auto& skewMode = result->params[(int)FFEffectParam::SkewMode];
  skewMode.acc = false;
  skewMode.defaultText = "Uni";
  skewMode.name = "Skew Mode";
  skewMode.display = "Mode";
  skewMode.slotCount = FFEffectBlockCount;
  skewMode.id = "{DCA38D64-3791-4542-A6C7-FCA66DA45FEE}";
  skewMode.type = FBParamType::List;
  skewMode.List().items = {
    { "{247BC86E-078E-409F-99B7-870F1B011C3B}", "Uni" },
    { "{C7689457-2AE9-4730-A341-5CB7B27047DE}", "Bi" } };
  auto selectSkewMode = [](auto& module) { return &module.block.skewMode; };
  skewMode.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSkewMode);
  skewMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSkewMode);
  skewMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSkewMode);
  skewMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Skew; });
  skewMode.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Skew; });

  auto& distDrive = result->params[(int)FFEffectParam::DistDrive];
  distDrive.acc = true;
  distDrive.defaultText = "100";
  distDrive.name = "Dist Drive";
  distDrive.display = "Drv";
  distDrive.slotCount = FFEffectBlockCount;
  distDrive.unit = "%";
  distDrive.id = "{971B9F5B-0348-4F56-A6A0-DC40FC4B32BD}";
  distDrive.type = FBParamType::Linear;
  distDrive.Linear().min = 0.0f;
  distDrive.Linear().max = 32.0f;
  distDrive.Linear().displayMultiplier = 100;
  auto selectDistDrive = [](auto& module) { return &module.acc.distDrive; };
  distDrive.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistDrive);
  distDrive.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistDrive);
  distDrive.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistDrive);
  distDrive.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distDrive.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] >= (int)FFEffectKind::Clip; });

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
  distMix.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distMix.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] >= (int)FFEffectKind::Clip; });

  auto& distBias = result->params[(int)FFEffectParam::DistBias];
  distBias.acc = true;
  distBias.defaultText = "0";
  distBias.name = "Dist Bias";
  distBias.display = "Bias";
  distBias.unit = "%";
  distBias.slotCount = FFEffectBlockCount;
  distBias.id = "{E3512478-1203-47D3-B5A3-F8BFBAAE264C}";
  distBias.type = FBParamType::Linear;
  distBias.Linear().min = -1.0f;
  distBias.Linear().max = 1.0f;
  distBias.Linear().displayMultiplier = 100.0f;
  auto selectDistBias = [](auto& module) { return &module.acc.distBias; };
  distBias.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistBias);
  distBias.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistBias);
  distBias.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistBias);
  distBias.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distBias.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] >= (int)FFEffectKind::Clip; });

  auto& distAmt = result->params[(int)FFEffectParam::DistAmt];
  distAmt.acc = true;
  distAmt.defaultText = "50";
  distAmt.name = "Dist Amt";
  distAmt.display = "Amt";
  distAmt.slotCount = FFEffectBlockCount;
  distAmt.id = "{C78B596F-8059-44F0-B73D-A699AB647F54}";
  distAmt.type = FBParamType::Identity;
  auto selectDistAmt = [](auto& module) { return &module.acc.distAmt; };
  distAmt.scalarAddr = FFSelectScalarParamAddr(selectModule, selectDistAmt);
  distAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectDistAmt);
  distAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectDistAmt);
  distAmt.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distAmt.dependencies.enabled.audio.When({ (int)FFEffectParam::Type, (int)FFEffectParam::Kind, (int)FFEffectParam::ClipMode },
    [](auto const& vs) { return vs[0] != 0 && (vs[1] == (int)FFEffectKind::Skew || vs[1] == (int)FFEffectKind::Clip && vs[2] == (int)FFEffectClipMode::Exp); });

  return result;
}