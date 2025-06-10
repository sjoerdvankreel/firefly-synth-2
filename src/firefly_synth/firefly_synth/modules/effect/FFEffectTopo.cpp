#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/dsp/shared/FFCombFilter.hpp>
#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectGraph.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeEffectTopo(bool global)
{
  std::string prefix = global ? "G" : ""; // TODO prefix should be V once we go out of alpha, but, dont want to refix the demos right now
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->name = global? "GFX": "VFX";
  result->slotCount = FFEffectCount;
  result->graphCount = FFEffectBlockCount + 1;
  result->graphRenderer = global? FFEffectRenderGraph<true>: FFEffectRenderGraph<false>;
  result->id = prefix + "{154051CE-66D9-41C8-B479-C52D1111C962}";
  result->params.resize((int)FFEffectParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vEffect; });
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gEffect; });
  auto selectVoiceModule = [](auto& state) { return &state.voice.vEffect; };
  auto selectGlobalModule = [](auto& state) { return &state.global.gEffect; };

  auto& on = result->params[(int)FFEffectParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.defaultText = "Off";
  on.id = prefix + "{85CEFF97-A98D-4470-BA15-825608A0E954}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOn);
  on.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOn);
  on.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOn);
  on.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOn);
  on.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOn);

  auto& oversample = result->params[(int)FFEffectParam::Oversample];
  oversample.acc = false;
  oversample.defaultText = "Off";
  oversample.name = "Ovrsmp";
  oversample.slotCount = 1;
  oversample.id = prefix + "{28875DF7-255B-4190-80CE-D0A9ED20F263}";
  oversample.type = FBParamType::Boolean;
  auto selectOversample = [](auto& module) { return &module.block.oversample; };
  oversample.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOversample);
  oversample.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOversample);
  oversample.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOversample);
  oversample.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOversample);
  oversample.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOversample);
  oversample.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& trackingKey = result->params[(int)FFEffectParam::TrackingKey];
  trackingKey.acc = true;
  trackingKey.defaultText = "0";
  trackingKey.name = "Tracking Key";
  trackingKey.display = "Key";
  trackingKey.slotCount = 1;
  trackingKey.unit = "Semitones";
  trackingKey.id = prefix + "{409B79DA-5F59-4C54-AA3D-3E7E84B1A303}";
  trackingKey.type = FBParamType::Linear;
  trackingKey.Linear().min = -64.0f;
  trackingKey.Linear().max = 64.0f;
  auto selectTrackingKey = [](auto& module) { return &module.acc.trackingKey; };
  trackingKey.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTrackingKey);
  trackingKey.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTrackingKey);
  trackingKey.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTrackingKey);
  trackingKey.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTrackingKey);
  trackingKey.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTrackingKey);
  trackingKey.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& kind = result->params[(int)FFEffectParam::Kind];
  kind.acc = false;
  kind.name = "Kind";
  kind.slotCount = FFEffectBlockCount;
  kind.id = prefix + "{1585A19D-639E-4202-B60B-BD8560BC8B70}";
  kind.defaultTextSelector = [](int ms, int ps) { return ps == 0 ? "StVar" : "Off"; };
  kind.type = FBParamType::List;
  kind.List().items = {
    { prefix + "{46A4BE9B-1072-4811-B3A1-3A463D0BA534}", "Off" },
    { prefix + "{348FED12-9753-4C48-9D21-BB8D21E036AB}", "StVar" },
    { prefix + "{B74E9485-459E-4017-ACF4-8466FBBF51EF}", "Comb" },
    { prefix + "{FD072A47-EE67-4091-A687-7168B69A6E89}", "Clip" },
    { prefix + "{06334343-5264-489E-ADF9-20ADCEF983FC}", "Fold" },
    { prefix + "{3DA2A1FC-6683-4F38-9443-18D9CBB7A684}", "Skew" } };
  kind.List().submenuStart[(int)FFEffectKind::Off] = "Off";
  kind.List().submenuStart[(int)FFEffectKind::StVar] = "Filter";
  kind.List().submenuStart[(int)FFEffectKind::Clip] = "Shape";
  auto selectKind = [](auto& module) { return &module.block.kind; };
  kind.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectKind);
  kind.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectKind);
  kind.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectKind);
  kind.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectKind);
  kind.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectKind);
  kind.dependencies.enabled.audio.When({ (int)FFEffectParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& stVarMode = result->params[(int)FFEffectParam::StVarMode];
  stVarMode.acc = false;
  stVarMode.defaultText = "LPF";
  stVarMode.name = "StVar Mode";
  stVarMode.display = "Mod";
  stVarMode.slotCount = FFEffectBlockCount;
  stVarMode.id = prefix + "{275B2C8D-6D21-4741-AB69-D21FA95CD7F5}";
  stVarMode.type = FBParamType::List;
  stVarMode.List().items = {
    { prefix + "{EAAE7102-9F6C-4EC2-8C39-B13BBDFF7AD1}", "LPF" },
    { prefix + "{6A91C381-DB9F-4CAA-8155-4A407700661A}", "BPF" },
    { prefix + "{747DA91C-C43D-4CFC-8EFD-353B0AC23E0E}", "HPF" },
    { prefix + "{10FEE670-AB90-4DBF-A617-6F15F3E4602D}", "BSF" },
    { prefix + "{EE9A4F79-B557-43B4-ABDF-320414D773D5}", "APF" },
    { prefix + "{C9C3A3F5-5C5B-4331-8F6E-DDD2DC5A1D7B}", "PEQ" },
    { prefix + "{8D885EEB-DF69-455A-9FFD-BA95E3E30596}", "BLL" },
    { prefix + "{C4BBC616-CFDB-4E93-9315-D25552D85F71}", "LSH" },
    { prefix + "{AF2550ED-90C5-4E09-996D-A4669728C744}", "HSH" } };
  auto selectStVarMode = [](auto& module) { return &module.block.stVarMode; };
  stVarMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectStVarMode);
  stVarMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectStVarMode);
  stVarMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectStVarMode);
  stVarMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectStVarMode);
  stVarMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectStVarMode);
  stVarMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

  auto& stVarKeyTrk = result->params[(int)FFEffectParam::StVarKeyTrak];
  stVarKeyTrk.acc = true;
  stVarKeyTrk.defaultText = "0";
  stVarKeyTrk.name = "StVar KeyTrk";
  stVarKeyTrk.display = "KTr";
  stVarKeyTrk.slotCount = FFEffectBlockCount;
  stVarKeyTrk.unit = "%";
  stVarKeyTrk.id = prefix + "{CC91F0B0-9D53-4140-B698-0561D04F500C}";
  stVarKeyTrk.type = FBParamType::Linear;
  stVarKeyTrk.Linear().min = -2.0f;
  stVarKeyTrk.Linear().max = 2.0f;
  stVarKeyTrk.Linear().displayMultiplier = 100;
  auto selectStVarKeyTrk = [](auto& module) { return &module.acc.stVarKeyTrk; };
  stVarKeyTrk.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectStVarKeyTrk);
  stVarKeyTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectStVarKeyTrk);
  stVarKeyTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectStVarKeyTrk);
  stVarKeyTrk.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectStVarKeyTrk);
  stVarKeyTrk.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectStVarKeyTrk);
  stVarKeyTrk.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarKeyTrk.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

  auto& stVarRes = result->params[(int)FFEffectParam::StVarRes];
  stVarRes.acc = true;
  stVarRes.defaultText = "0";
  stVarRes.name = "StVar Res";
  stVarRes.display = "Res";
  stVarRes.slotCount = FFEffectBlockCount;
  stVarRes.unit = "%";
  stVarRes.id = prefix + "{0B7CCB1C-FF95-46CD-9C93-1BAF5CE350E3}";
  stVarRes.type = FBParamType::Identity;
  auto selectStVarRes = [](auto& module) { return &module.acc.stVarRes; };
  stVarRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectStVarRes);
  stVarRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectStVarRes);
  stVarRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectStVarRes);
  stVarRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectStVarRes);
  stVarRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectStVarRes);
  stVarRes.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarRes.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

  auto& stVarFreq = result->params[(int)FFEffectParam::StVarFreq];
  stVarFreq.acc = true;
  stVarFreq.defaultText = "1000";
  stVarFreq.name = "StVar Freq";
  stVarFreq.display = "Frq";
  stVarFreq.slotCount = FFEffectBlockCount;
  stVarFreq.unit = "Hz";
  stVarFreq.id = prefix + "{4ACF9D3E-D1F0-4B3B-8F6F-6FEE6BCDE449}";
  stVarFreq.type = FBParamType::Log2;
  stVarFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectStVarFreq = [](auto& module) { return &module.acc.stVarFreq; };
  stVarFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectStVarFreq);
  stVarFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectStVarFreq);
  stVarFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectStVarFreq);
  stVarFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectStVarFreq);
  stVarFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectStVarFreq);
  stVarFreq.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarFreq.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar; });

  auto& stVarGain = result->params[(int)FFEffectParam::StVarGain];
  stVarGain.acc = true;
  stVarGain.defaultText = "0";
  stVarGain.name = "StVar Gain";
  stVarGain.display = "Gn";
  stVarGain.slotCount = FFEffectBlockCount;
  stVarGain.unit = "dB";
  stVarGain.id = prefix + "{CA06747B-F867-4E03-AF36-327662021440}";
  stVarGain.type = FBParamType::Linear;
  stVarGain.Linear().min = -24.0f;
  stVarGain.Linear().max = 24.0f;
  auto selectStVarGain = [](auto& module) { return &module.acc.stVarGain; };
  stVarGain.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectStVarGain);
  stVarGain.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectStVarGain);
  stVarGain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectStVarGain);
  stVarGain.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectStVarGain);
  stVarGain.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectStVarGain);
  stVarGain.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == 0 || vs[0] == (int)FFEffectKind::StVar; });
  stVarGain.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind, (int)FFEffectParam::StVarMode},
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::StVar && vs[2] >= (int)FFStateVariableFilterMode::BLL; });

  auto& combKeyTrk = result->params[(int)FFEffectParam::CombKeyTrk];
  combKeyTrk.acc = true;
  combKeyTrk.defaultText = "0";
  combKeyTrk.name = "Comb KeyTrk";
  combKeyTrk.display = "KTr";
  combKeyTrk.slotCount = FFEffectBlockCount;
  combKeyTrk.unit = "%";
  combKeyTrk.id = prefix + "{77B1716F-4511-492B-A32E-F04CF668238B}";
  combKeyTrk.type = FBParamType::Linear;
  combKeyTrk.Linear().min = -2.0f;
  combKeyTrk.Linear().max = 2.0f;
  combKeyTrk.Linear().displayMultiplier = 100;
  auto selectCombKeyTrk = [](auto& module) { return &module.acc.combKeyTrk; };
  combKeyTrk.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectCombKeyTrk);
  combKeyTrk.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectCombKeyTrk);
  combKeyTrk.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectCombKeyTrk);
  combKeyTrk.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectCombKeyTrk);
  combKeyTrk.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectCombKeyTrk);
  combKeyTrk.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combKeyTrk.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });
  
  auto& combFreqPlus = result->params[(int)FFEffectParam::CombFreqPlus];
  combFreqPlus.acc = true;
  combFreqPlus.defaultText = "1000";
  combFreqPlus.name = "Comb Freq+";
  combFreqPlus.display = "Frq+";
  combFreqPlus.slotCount = FFEffectBlockCount;
  combFreqPlus.unit = "Hz";
  combFreqPlus.id = prefix + "{19B8B573-C49D-4D34-8078-02B2A30F40E8}";
  combFreqPlus.type = FBParamType::Log2;
  combFreqPlus.Log2().Init(0.0f, FFMinCombFilterFreq, FFMaxCombFilterFreq);
  auto selectCombFreqPlus = [](auto& module) { return &module.acc.combFreqPlus; };
  combFreqPlus.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectCombFreqPlus);
  combFreqPlus.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectCombFreqPlus);
  combFreqPlus.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectCombFreqPlus);
  combFreqPlus.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectCombFreqPlus);
  combFreqPlus.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectCombFreqPlus);
  combFreqPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combFreqPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

  auto& combFreqMin = result->params[(int)FFEffectParam::CombFreqMin];
  combFreqMin.acc = true;
  combFreqMin.defaultText = "1000";
  combFreqMin.name = "Comb Freq-";
  combFreqMin.display = "Frq-";
  combFreqMin.slotCount = FFEffectBlockCount;
  combFreqMin.unit = "Hz";
  combFreqMin.id = prefix + "{B2C0AEB8-F2C1-4553-961C-7B61021C8B70}";
  combFreqMin.type = FBParamType::Log2;
  combFreqMin.Log2().Init(0.0f, FFMinCombFilterFreq, FFMaxCombFilterFreq);
  auto selectCombFreqMin = [](auto& module) { return &module.acc.combFreqMin; };
  combFreqMin.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectCombFreqMin);
  combFreqMin.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectCombFreqMin);
  combFreqMin.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectCombFreqMin);
  combFreqMin.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectCombFreqMin);
  combFreqMin.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectCombFreqMin);
  combFreqMin.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combFreqMin.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

  auto& combResPlus = result->params[(int)FFEffectParam::CombResPlus];
  combResPlus.acc = true;
  combResPlus.defaultText = "50";
  combResPlus.name = "Comb Rz+";
  combResPlus.display = "Res+";
  combResPlus.slotCount = FFEffectBlockCount;
  combResPlus.unit = "%";
  combResPlus.id = prefix + "{7DF779CB-794D-4419-9E7F-E68F3F3BCB57}";
  combResPlus.type = FBParamType::Linear;
  combResPlus.Linear().min = -1.0f;
  combResPlus.Linear().max = 1.0f;
  combResPlus.Linear().displayMultiplier = 100;
  auto selectCombResPlus = [](auto& module) { return &module.acc.combResPlus; };
  combResPlus.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectCombResPlus);
  combResPlus.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectCombResPlus);
  combResPlus.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectCombResPlus);
  combResPlus.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectCombResPlus);
  combResPlus.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectCombResPlus);
  combResPlus.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combResPlus.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

  auto& combResMin = result->params[(int)FFEffectParam::CombResMin];
  combResMin.acc = true;
  combResMin.defaultText = "0";
  combResMin.name = "Comb Res-";
  combResMin.display = "Res-";
  combResMin.slotCount = FFEffectBlockCount;
  combResMin.unit = "%";
  combResMin.id = prefix + "{09588739-10E7-413E-9577-5A9BE8996A5D}";
  combResMin.type = FBParamType::Linear;
  combResMin.Linear().min = -1.0f;
  combResMin.Linear().max = 1.0f;
  combResMin.Linear().displayMultiplier = 100;
  auto selectCombResMin = [](auto& module) { return &module.acc.combResMin; };
  combResMin.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectCombResMin);
  combResMin.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectCombResMin);
  combResMin.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectCombResMin);
  combResMin.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectCombResMin);
  combResMin.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectCombResMin);
  combResMin.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Comb; });
  combResMin.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Comb; });

  auto& clipMode = result->params[(int)FFEffectParam::ClipMode];
  clipMode.acc = false;
  clipMode.defaultText = "TanH";
  clipMode.name = "Clip Mode";
  clipMode.display = "Mod";
  clipMode.slotCount = FFEffectBlockCount;
  clipMode.id = prefix + "{D1F80BB8-4076-4296-A678-94E8442C51A5}";
  clipMode.type = FBParamType::List;
  clipMode.List().items = {
    { prefix + "{32F53B15-54AC-44AE-8812-97D598B9928B}", "Hard" },
    { prefix + "{E4ECBDA0-F14F-411D-81A8-C59CC9B7C2C6}", "TanH" },
    { prefix + "{851F55D9-89E0-4B37-A6DA-A81692A716BD}", "Sin" },
    { prefix + "{A629BC3E-4732-4A6A-AB79-38CE84F04B0D}", "TSQ" },
    { prefix + "{C0E30CBB-596C-4267-96E9-9FBFD5D26C27}", "Cube" },
    { prefix + "{192BF63E-663D-494C-956F-3A8BB2E22067}", "Inv" },
    { prefix + "{8A58AAB2-0AE7-426E-B71F-A444653286A6}", "Exp" } };
  auto selectClipMode = [](auto& module) { return &module.block.clipMode; };
  clipMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectClipMode);
  clipMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectClipMode);
  clipMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectClipMode);
  clipMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectClipMode);
  clipMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectClipMode);
  clipMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Clip; });
  clipMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Clip; });

  // todo submenu
  auto& foldMode = result->params[(int)FFEffectParam::FoldMode];
  foldMode.acc = false;
  foldMode.defaultText = "Sin";
  foldMode.name = "Fold Mode";
  foldMode.display = "Mod";
  foldMode.slotCount = FFEffectBlockCount;
  foldMode.id = prefix + "{317BA4AC-8E9A-47B9-A289-294047E29C78}";
  foldMode.type = FBParamType::List;
  foldMode.List().items = {
    { prefix + "{55167ED0-D050-403B-A061-CA4D0916E400}", "Fold" },
    { prefix + "{129369F6-C303-4BBA-8573-06FC33972FD9}", "Sin" },
    { prefix + "{8CFCDA01-C9A9-4231-9994-8480CC08A1CE}", "Cos" },
    { prefix + "{549CC93F-C88A-4C3B-AD37-B3C818DFF573}", "Sin2" },
    { prefix + "{544151DB-5403-4732-B416-6CA2C9C78066}", "Cos2" },
    { prefix + "{C10D2D9B-53F1-4779-AE80-D463A0DD7278}", "SnCs" },
    { prefix + "{D820E334-2D19-4306-914D-00AA7D048D48}", "CsSn" },
    { prefix + "{BB19624C-893C-41F9-83AD-CB17DFD9FC60}", "Sin3" },
    { prefix + "{66737A7F-6548-4881-B8F3-69FCF0EB1843}", "Cos3" },
    { prefix + "{2133F2BE-95B6-4845-A622-5712F2747960}", "S2Cs" },
    { prefix + "{5C572DE3-6197-4289-A009-573A88E2B09F}", "C2Sn" },
    { prefix + "{7527549D-68FE-4D6F-B420-BA75F9097EEE}", "SnC2" },
    { prefix + "{7DA4D108-2DCB-49C1-97D3-A3528A3BD715}", "CsS2" },
    { prefix + "{EAEFCA78-2779-484D-AC67-CD61786B64B5}", "SCS" },
    { prefix + "{4C0E5578-38F2-411C-A266-8FD9FFEA8612}", "CSC" } };
  auto selectFoldMode = [](auto& module) { return &module.block.foldMode; };
  foldMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFoldMode);
  foldMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFoldMode);
  foldMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFoldMode);
  foldMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFoldMode);
  foldMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFoldMode);
  foldMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Fold; });
  foldMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Fold; });
  
  auto& skewMode = result->params[(int)FFEffectParam::SkewMode];
  skewMode.acc = false;
  skewMode.defaultText = "Uni";
  skewMode.name = "Skew Mode";
  skewMode.display = "Mod";
  skewMode.slotCount = FFEffectBlockCount;
  skewMode.id = prefix + "{DCA38D64-3791-4542-A6C7-FCA66DA45FEE}";
  skewMode.type = FBParamType::List;
  skewMode.List().items = {
    { prefix + "{247BC86E-078E-409F-99B7-870F1B011C3B}", "Uni" },
    { prefix + "{C7689457-2AE9-4730-A341-5CB7B27047DE}", "Bi" } };
  auto selectSkewMode = [](auto& module) { return &module.block.skewMode; };
  skewMode.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSkewMode);
  skewMode.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSkewMode);
  skewMode.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSkewMode);
  skewMode.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSkewMode);
  skewMode.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSkewMode);
  skewMode.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] == (int)FFEffectKind::Skew; });
  skewMode.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] == (int)FFEffectKind::Skew; });

  auto& distDrive = result->params[(int)FFEffectParam::DistDrive];
  distDrive.acc = true;
  distDrive.defaultText = "100";
  distDrive.name = "Dist Drive";
  distDrive.display = "Drv";
  distDrive.slotCount = FFEffectBlockCount;
  distDrive.unit = "%";
  distDrive.id = prefix + "{971B9F5B-0348-4F56-A6A0-DC40FC4B32BD}";
  distDrive.type = FBParamType::Linear;
  distDrive.Linear().min = 0.0f;
  distDrive.Linear().max = 32.0f;
  distDrive.Linear().displayMultiplier = 100;
  auto selectDistDrive = [](auto& module) { return &module.acc.distDrive; };
  distDrive.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDistDrive);
  distDrive.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDistDrive);
  distDrive.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDistDrive);
  distDrive.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDistDrive);
  distDrive.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDistDrive);
  distDrive.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distDrive.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] >= (int)FFEffectKind::Clip; });

  auto& distMix = result->params[(int)FFEffectParam::DistMix];
  distMix.acc = true;
  distMix.defaultText = "100";
  distMix.name = "Dist Mix";
  distMix.display = "Mix";
  distMix.slotCount = FFEffectBlockCount;
  distMix.id = prefix + "{CD542E15-A8DD-4A72-9B75-E8D8301D8F05}";
  distMix.type = FBParamType::Identity;
  auto selectDistMix = [](auto& module) { return &module.acc.distMix; };
  distMix.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDistMix);
  distMix.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDistMix);
  distMix.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDistMix);
  distMix.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDistMix);
  distMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDistMix);
  distMix.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distMix.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] >= (int)FFEffectKind::Clip; });

  auto& distBias = result->params[(int)FFEffectParam::DistBias];
  distBias.acc = true;
  distBias.defaultText = "0";
  distBias.name = "Dist Bias";
  distBias.display = "Bias";
  distBias.unit = "%";
  distBias.slotCount = FFEffectBlockCount;
  distBias.id = prefix + "{E3512478-1203-47D3-B5A3-F8BFBAAE264C}";
  distBias.type = FBParamType::Linear;
  distBias.Linear().min = -1.0f;
  distBias.Linear().max = 1.0f;
  distBias.Linear().displayMultiplier = 100.0f;
  auto selectDistBias = [](auto& module) { return &module.acc.distBias; };
  distBias.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDistBias);
  distBias.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDistBias);
  distBias.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDistBias);
  distBias.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDistBias);
  distBias.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDistBias);
  distBias.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distBias.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] != 0 && vs[1] >= (int)FFEffectKind::Clip; });

  auto& distAmt = result->params[(int)FFEffectParam::DistAmt];
  distAmt.acc = true;
  distAmt.defaultText = "50";
  distAmt.name = "Dist Amt";
  distAmt.display = "Amt";
  distAmt.slotCount = FFEffectBlockCount;
  distAmt.id = prefix + "{C78B596F-8059-44F0-B73D-A699AB647F54}";
  distAmt.type = FBParamType::Identity;
  auto selectDistAmt = [](auto& module) { return &module.acc.distAmt; };
  distAmt.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDistAmt);
  distAmt.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDistAmt);
  distAmt.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDistAmt);
  distAmt.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDistAmt);
  distAmt.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDistAmt);
  distAmt.dependencies.visible.audio.When({ (int)FFEffectParam::Kind },
    [](auto const& vs) { return vs[0] >= (int)FFEffectKind::Clip; });
  distAmt.dependencies.enabled.audio.When({ (int)FFEffectParam::On, (int)FFEffectParam::Kind, (int)FFEffectParam::ClipMode },
    [](auto const& vs) { return vs[0] != 0 && (vs[1] == (int)FFEffectKind::Skew || vs[1] == (int)FFEffectKind::Clip && vs[2] == (int)FFEffectClipMode::Exp); });

  return result;
}