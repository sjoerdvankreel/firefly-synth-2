#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoGraph.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeEchoBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 4, 1 });
}

static std::vector<FBBarsItem>
MakeEchoSmoothBarsItems()
{
  return FBMakeBarsItems(false, { 1, 16 }, { 4, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeEchoTopo(bool global)
{
  std::string prefix = global ? "G" : "V";
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->name = global? "GEcho": "VEcho";
  result->slotCount = 1;
  result->graphCount = 4;
  result->graphRenderer = global ? FFEchoRenderGraph<true>: FFEchoRenderGraph<false>;
  result->id = prefix + "{B979D7BD-65A2-42E4-A7B2-3A48BBFFDE23}";
  result->params.resize((int)FFEchoParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vEcho; });
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gEcho; });
  auto selectGuiVoiceModule = [](auto& state) { return &state.vEcho; };
  auto selectGuiGlobalModule = [](auto& state) { return &state.gEcho; };
  auto selectVoiceModule = [](auto& state) { return &state.voice.vEcho; };
  auto selectGlobalModule = [](auto& state) { return &state.global.gEcho; };

  auto& vTargetOrGTarget = result->params[(int)FFEchoParam::VTargetOrGTarget];
  vTargetOrGTarget.mode = FBParamMode::Block;
  vTargetOrGTarget.slotCount = 1;
  vTargetOrGTarget.id = prefix + "{60F7D173-C5F9-46AD-A108-D17D40FE4C1D}";
  vTargetOrGTarget.name = "Target";
  vTargetOrGTarget.defaultText = "Off";
  vTargetOrGTarget.type = FBParamType::List;
  auto selectVTargetOrGTarget = [](auto& module) { return &module.block.vTargetOrGTarget; };
  vTargetOrGTarget.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVTargetOrGTarget);
  vTargetOrGTarget.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVTargetOrGTarget);
  vTargetOrGTarget.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVTargetOrGTarget);
  vTargetOrGTarget.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVTargetOrGTarget);
  vTargetOrGTarget.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVTargetOrGTarget);
  if (global)
    vTargetOrGTarget.List().items = {
      { "{90701058-1399-4D0A-B098-AE5AFFB9123C}", "Off" },
      { "{38A69D55-AB3E-49F2-9CCD-44917FD24597}", "GMix In" },
      { "{C510F9C1-13A6-4929-A7EA-7C5F79BA6E42}", "GMix Out" },
      { "{9A195809-A9A8-454A-ACD1-5376892EE416}", "Voice Mix" },
      { "{CDE64B50-C65D-43E9-B52E-325F544E5175}", "GFX1 In" },
      { "{1C7AA2CC-2116-45E4-B7C2-AA9678CC15B5}", "GFX1 Out" },
      { "{68C1469F-96FF-47D7-8A2E-B24309E10031}", "GFX2 In" },
      { "{A043114B-C471-46A8-A526-B93E1897B1A0}", "GFX2 Out" },
      { "{883FA130-AB68-42D5-B56A-C61039DAD9A6}", "GFX3 In" },
      { "{56CE90EE-F9D0-4CCF-93B8-9BB0501DCBEF}", "GFX3 Out" },
      { "{80ECC9AA-AC6B-44DC-B305-43057720A4AC}", "GFX4 In" },
      { "{050E8D54-BFD2-4821-B14F-717D161D69DB}", "GFX4 Out" } };
  else
    vTargetOrGTarget.List().items = {
      { "{0EFCD973-BAA5-47B9-B61B-23FC8B82DC03}", "Off" },
      { "{41E82846-7503-4DBA-B2B2-880482DE7970}", "VMIx In" },
      { "{4E37CD52-00B3-455A-9958-99F7ACBE544E}", "VMix Out" },
      { "{CFD09E84-2938-4113-867F-31941D722578}", "Osc Mix" },
      { "{26FABB4C-7E94-4CAF-ABC3-7ED3610D514E}", "Osc 1 PreMix" },
      { "{12F8A4E6-7D4F-4E7E-80AC-3502B801C76E}", "Osc 1 PostMix" },
      { "{66805E87-E1B2-4B1C-87F1-487CAF824EE7}", "Osc 2 PreMix" },
      { "{75EB8B97-D30E-4912-BDDD-656E267FD698}", "Osc 2 PostMix" },
      { "{6FB91E2B-179B-43BA-BBD8-7C1DDF2EDD15}", "Osc 3 PreMix" },
      { "{E3C5A47A-68D7-4FF5-BEE4-9C022736263C}", "Osc 3 PostMix" },
      { "{3004C2C5-BBE2-47F1-93AE-C4E5CE4618C7}", "Osc 4 PreMix" },
      { "{C42EFDE3-444D-42EA-906D-FE5563E86250}", "Osc 4 PostMix" },
      { "{76D95D3E-0D97-434A-B960-A7BD3667F57D}", "VFX1 In" },
      { "{F7432F58-5DA1-45DD-9B02-90D32ABE6B3F}", "VFX1 Out" },
      { "{371838D8-5D97-459E-A553-29CB416793DE}", "VFX2 In" },
      { "{4379BE69-288E-4C96-81C0-10BF312E73C9}", "VFX2 Out" },
      { "{CBA10ED2-7C2F-4312-BBD9-45957AD840D7}", "VFX3 In" },
      { "{F2E5F511-BC7C-4759-9FCC-98E369E6B641}", "VFX3 Out" },
      { "{D9140819-6982-4B2C-883E-0D6325815FB8}", "VFX4 In" },
      { "{86AA4803-EA25-4C52-B2AB-0CEEA44BE0C0}", "VFX4 Out" } };

  auto& order = result->params[(int)FFEchoParam::Order];
  order.mode = FBParamMode::Block;
  order.slotCount = 1;
  order.name = "Order";
  order.defaultText = "T\U00002192F\U00002192R";
  order.id = prefix + "{D84A623A-7F5C-40C6-A892-A441497783CD}";
  order.type = FBParamType::List;
  order.List().items = {
    { "{90701058-1399-4D0A-B098-AE5AFFB9123C}", "T\U00002192F\U00002192R" },
    { "{9A195809-A9A8-454A-ACD1-5376892EE416}", "T\U00002192R\U00002192F" },
    { "{883FA130-AB68-42D5-B56A-C61039DAD9A6}", "F\U00002192T\U00002192R" },
    { "{56CE90EE-F9D0-4CCF-93B8-9BB0501DCBEF}", "F\U00002192R\U00002192T" },
    { "{80ECC9AA-AC6B-44DC-B305-43057720A4AC}", "R\U00002192T\U00002192F" },
    { "{050E8D54-BFD2-4821-B14F-717D161D69DB}", "R\U00002192F\U00002192T" } };
  auto selectOrder = [](auto& module) { return &module.block.order; };
  order.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOrder);
  order.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOrder);
  order.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOrder);
  order.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOrder);
  order.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOrder);
  order.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& gain = result->params[(int)FFEchoParam::Gain];
  gain.mode = FBParamMode::Accurate;
  gain.defaultText = "200";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "%";
  gain.id = prefix + "{557D9FBC-0EA3-4DDD-914F-7C9E647E25E0}";
  gain.type = FBParamType::Linear;
  gain.Linear().min = 0.0f;
  gain.Linear().max = 4.0f;
  gain.Linear().displayMultiplier = 100.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectGain);
  gain.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectGain);
  gain.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectGain);
  gain.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectGain);
  gain.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectGain);
  gain.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& sync = result->params[(int)FFEchoParam::Sync];
  sync.mode = FBParamMode::Block;
  sync.name = "Tempo Sync";
  sync.display = "Sync";
  sync.slotCount = 1;
  sync.id = prefix + "{6CBE8845-0C59-4A95-B9F7-24C7AA061DFA}";
  sync.defaultText = "Off";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSync);
  sync.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSync);
  sync.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSync);
  sync.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSync);
  sync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSync);
  sync.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& delaySmoothTime = result->params[(int)FFEchoParam::DelaySmoothTime];
  delaySmoothTime.mode = FBParamMode::Block;
  delaySmoothTime.defaultText = "500";
  delaySmoothTime.display = "Smth";
  delaySmoothTime.name = "Delay Smooth Time";
  delaySmoothTime.slotCount = 1;
  delaySmoothTime.unit = "Ms";
  delaySmoothTime.id = prefix + "{8694FBC8-003F-47B3-BB0E-8FB610CD4BD1}";
  delaySmoothTime.type = FBParamType::Linear;
  delaySmoothTime.Linear().min = 0.02f;
  delaySmoothTime.Linear().max = 5.0f;
  delaySmoothTime.Linear().editSkewFactor = 0.5f;
  delaySmoothTime.Linear().displayMultiplier = 1000;
  auto selectDelaySmoothTime = [](auto& module) { return &module.block.delaySmoothTime; };
  delaySmoothTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDelaySmoothTime);
  delaySmoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDelaySmoothTime);
  delaySmoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDelaySmoothTime);
  delaySmoothTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDelaySmoothTime);
  delaySmoothTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDelaySmoothTime);
  delaySmoothTime.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  delaySmoothTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& delaySmoothBars = result->params[(int)FFEchoParam::DelaySmoothBars];
  delaySmoothBars.mode = FBParamMode::Block;
  delaySmoothBars.display = "Smth";
  delaySmoothBars.name = "Delay Smooth Bars";
  delaySmoothBars.slotCount = 1;
  delaySmoothBars.unit = "Bars";
  delaySmoothBars.id = prefix + "{5554CC4B-29E6-4F3C-8FFE-95947033676C}";
  delaySmoothBars.defaultText = "1/4";
  delaySmoothBars.type = FBParamType::Bars;
  delaySmoothBars.Bars().items = MakeEchoSmoothBarsItems();
  auto selectDelaySmoothBars = [](auto& module) { return &module.block.delaySmoothBars; };
  delaySmoothBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDelaySmoothBars);
  delaySmoothBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDelaySmoothBars);
  delaySmoothBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDelaySmoothBars);
  delaySmoothBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDelaySmoothBars);
  delaySmoothBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDelaySmoothBars);
  delaySmoothBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  delaySmoothBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  // only used for voice, but we still share the gui
  auto& voiceExtendTime = result->params[(int)FFEchoParam::VoiceExtendTime];
  voiceExtendTime.mode = FBParamMode::VoiceStart;
  voiceExtendTime.defaultText = "1000";
  voiceExtendTime.display = "Extend";
  voiceExtendTime.name = "Voice Extend Time";
  voiceExtendTime.slotCount = 1;
  voiceExtendTime.unit = "Ms";
  voiceExtendTime.id = prefix + "{1FD9788A-41D1-4705-AF68-29956B1337E1}";
  voiceExtendTime.type = FBParamType::Linear;
  voiceExtendTime.Linear().min = 0.0f;
  voiceExtendTime.Linear().max = 5.0f;
  voiceExtendTime.Linear().editSkewFactor = 0.5f;
  voiceExtendTime.Linear().displayMultiplier = 1000;
  auto selectVoiceExtendTime = [](auto& module) { return &module.voiceStart.voiceExtendTime; };
  voiceExtendTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVoiceExtendTime);
  voiceExtendTime.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVoiceExtendTime);
  voiceExtendTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVoiceExtendTime);
  voiceExtendTime.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVoiceExtendTime);
  voiceExtendTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVoiceExtendTime);
  voiceExtendTime.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  voiceExtendTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::Sync }, [global](auto const& vs) { return !global && vs[0] != 0 && vs[1] == 0; });

  auto& voiceExtendBars = result->params[(int)FFEchoParam::VoiceExtendBars];
  voiceExtendBars.mode = FBParamMode::Block;
  voiceExtendBars.display = "Extend";
  voiceExtendBars.name = "Voice Extend Bars";
  voiceExtendBars.slotCount = 1;
  voiceExtendBars.unit = "Bars";
  voiceExtendBars.id = prefix + "{4BFA9D36-87B5-403F-A05F-CB9D1A9D874E}";
  voiceExtendBars.defaultText = "1/2";
  voiceExtendBars.type = FBParamType::Bars;
  voiceExtendBars.Bars().items = MakeEchoBarsItems();
  auto selectVoiceExtendBars = [](auto& module) { return &module.block.voiceExtendBars; };
  voiceExtendBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVoiceExtendBars);
  voiceExtendBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVoiceExtendBars);
  voiceExtendBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVoiceExtendBars);
  voiceExtendBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVoiceExtendBars);
  voiceExtendBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVoiceExtendBars);
  voiceExtendBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  voiceExtendBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::Sync }, [global](auto const& vs) { return !global && vs[0] != 0 && vs[1] != 0; });
  
  auto& voiceFadeTime = result->params[(int)FFEchoParam::VoiceFadeTime];
  voiceFadeTime.mode = FBParamMode::VoiceStart;
  voiceFadeTime.defaultText = "1000";
  voiceFadeTime.display = "Fade";
  voiceFadeTime.name = "Voice Fade Time";
  voiceFadeTime.slotCount = 1;
  voiceFadeTime.unit = "Ms";
  voiceFadeTime.id = prefix + "{13DB349F-81B4-415D-B79D-F742632E0036}";
  voiceFadeTime.type = FBParamType::Linear;
  voiceFadeTime.Linear().min = 0.0f;
  voiceFadeTime.Linear().max = 5.0f;
  voiceFadeTime.Linear().editSkewFactor = 0.5f;
  voiceFadeTime.Linear().displayMultiplier = 1000;
  auto selectVoiceFadeTime = [](auto& module) { return &module.voiceStart.voiceFadeTime; };
  voiceFadeTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVoiceFadeTime);
  voiceFadeTime.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVoiceFadeTime);
  voiceFadeTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVoiceFadeTime);
  voiceFadeTime.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVoiceFadeTime);
  voiceFadeTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVoiceFadeTime);
  voiceFadeTime.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  voiceFadeTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::Sync }, [global](auto const& vs) { return !global && vs[0] != 0 && vs[1] == 0; });

  auto& voiceFadeBars = result->params[(int)FFEchoParam::VoiceFadeBars];
  voiceFadeBars.mode = FBParamMode::Block;
  voiceFadeBars.display = "Fade";
  voiceFadeBars.name = "Voice Fade Bars";
  voiceFadeBars.slotCount = 1;
  voiceFadeBars.unit = "Bars";
  voiceFadeBars.id = prefix + "{FB75B598-21C8-4D33-B296-F471F1AFD95A}";
  voiceFadeBars.defaultText = "1/2";
  voiceFadeBars.type = FBParamType::Bars;
  voiceFadeBars.Bars().items = MakeEchoBarsItems();
  auto selectVoiceFadeBars = [](auto& module) { return &module.block.voiceFadeBars; };
  voiceFadeBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVoiceFadeBars);
  voiceFadeBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVoiceFadeBars);
  voiceFadeBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVoiceFadeBars);
  voiceFadeBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVoiceFadeBars);
  voiceFadeBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVoiceFadeBars);
  voiceFadeBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  voiceFadeBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::Sync }, [global](auto const& vs) { return !global && vs[0] != 0 && vs[1] != 0; });

  auto& tapsMix = result->params[(int)FFEchoParam::TapsMix];
  tapsMix.mode = FBParamMode::Accurate;
  tapsMix.unit = "%";
  tapsMix.defaultText = "33";
  tapsMix.name = "Taps Mix";
  tapsMix.display = "Mix";
  tapsMix.slotCount = 1;
  tapsMix.id = prefix + "{56DF16E6-A079-4E97-92D9-DA65826DB20F}";
  tapsMix.type = FBParamType::Identity;
  auto selectTapsMix = [](auto& module) { return &module.acc.tapsMix; };
  tapsMix.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapsMix);
  tapsMix.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapsMix);
  tapsMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapsMix);
  tapsMix.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapsMix);
  tapsMix.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapsMix);
  tapsMix.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& tapsOn = result->params[(int)FFEchoParam::TapsOn];
  tapsOn.mode = FBParamMode::Block;
  tapsOn.name = "Taps On";
  tapsOn.display = "Taps";
  tapsOn.slotCount = 1;
  tapsOn.defaultText = "On";
  tapsOn.id = prefix + "{C5FAD167-9D4B-4640-AD46-0C3B55089197}";
  tapsOn.type = FBParamType::Boolean;
  auto selectTapsOn = [](auto& module) { return &module.block.tapsOn; };
  tapsOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapsOn);
  tapsOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapsOn);
  tapsOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapsOn);
  tapsOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapsOn);
  tapsOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapsOn);
  tapsOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& tapOn = result->params[(int)FFEchoParam::TapOn];
  tapOn.mode = FBParamMode::Block;
  tapOn.name = "Tap On";
  tapOn.display = "Tap";
  tapOn.slotCount = FFEchoTapCount;
  tapOn.defaultTextSelector = [](int, int, int ps) { return ps < 4 ? "On" : "Off"; };
  tapOn.id = prefix + "{0EA26FE5-F45F-431E-9421-0FDD9E508CB8}";
  tapOn.type = FBParamType::Boolean;
  tapOn.slotFormatDisplay = true;
  auto selectTapOn = [](auto& module) { return &module.block.tapOn; };
  tapOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapOn);
  tapOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapOn);
  tapOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapOn);
  tapOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapOn);
  tapOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapOn);
  tapOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& tapLevel = result->params[(int)FFEchoParam::TapLevel];
  tapLevel.mode = FBParamMode::Accurate;
  tapLevel.unit = "%";
  tapLevel.name = "Tap Level";
  tapLevel.display = "Lvl";
  tapLevel.slotCount = FFEchoTapCount;
  tapLevel.id = prefix + "{E3633411-F79D-4D2A-A748-82E03A35434E}";
  tapLevel.defaultTextSelector = [](int, int, int ps) { return ps == 0? "100": ps == 1? "80": ps == 2? "60": ps == 3? "40": "0"; };
  tapLevel.type = FBParamType::Identity;
  auto selectTapLevel = [](auto& module) { return &module.acc.tapLevel; };
  tapLevel.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapLevel);
  tapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapLevel);
  tapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapLevel);
  tapLevel.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapLevel);
  tapLevel.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapLevel);
  tapLevel.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });
    
  auto& tapDelayTime = result->params[(int)FFEchoParam::TapDelayTime];
  tapDelayTime.mode = FBParamMode::Accurate;
  tapDelayTime.display = "Dly";
  tapDelayTime.name = "Tap Delay Time";
  tapDelayTime.matrixName = "Tap Delay";
  tapDelayTime.slotCount = FFEchoTapCount;
  tapDelayTime.unit = "Sec";
  tapDelayTime.id = prefix + "{7ADA9075-213B-4809-B32E-39B2794B010F}";
  tapDelayTime.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "0.5" : ps == 1 ? "1" : ps == 2 ? "1.5" : ps == 3 ? "2" : "0"; };
  tapDelayTime.type = FBParamType::Linear;
  tapDelayTime.Linear().min = 0.0f;
  tapDelayTime.Linear().max = 10.0f;
  tapDelayTime.Linear().editSkewFactor = 0.5f;
  auto selectTapDelayTime = [](auto& module) { return &module.acc.tapDelayTime; };
  tapDelayTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapDelayTime);
  tapDelayTime.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapDelayTime);
  tapDelayTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapDelayTime);
  tapDelayTime.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapDelayTime);
  tapDelayTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapDelayTime);
  tapDelayTime.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  tapDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] == 0; });

  auto& tapDelayBars = result->params[(int)FFEchoParam::TapDelayBars];
  tapDelayBars.mode = FBParamMode::Block;
  tapDelayBars.display = "Dly";
  tapDelayBars.name = "Tap Delay Bars";
  tapDelayBars.slotCount = FFEchoTapCount;
  tapDelayBars.unit = "Bars";
  tapDelayBars.id = prefix + "{BEDF76D3-211D-4A1F-AF42-85E9C4E5374F}";
  tapDelayBars.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "1/4" : ps == 1 ? "1/2" : ps == 2 ? "3/4" : ps == 3 ? "1/1" : "Off"; };
  tapDelayBars.type = FBParamType::Bars;
  tapDelayBars.Bars().items = MakeEchoBarsItems();
  auto selectTapDelayBars = [](auto& module) { return &module.block.tapDelayBars; };
  tapDelayBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapDelayBars);
  tapDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapDelayBars);
  tapDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapDelayBars);
  tapDelayBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapDelayBars);
  tapDelayBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapDelayBars);
  tapDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  tapDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] != 0; });
   
  auto& tapBal = result->params[(int)FFEchoParam::TapBalance];
  tapBal.mode = FBParamMode::Accurate;
  tapBal.name = "Tap Balance";
  tapBal.matrixName = "Tap Bal";
  tapBal.display = "Bal";
  tapBal.slotCount = FFEchoTapCount;
  tapBal.unit = "%";
  tapBal.id = prefix + "{2DCCEE86-4381-4E7F-98B6-FA94059F253B}";
  tapBal.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "-33" : ps == 1 ? "33" : ps == 2 ? "-67" : ps == 3 ? "67" : "0"; };
  tapBal.type = FBParamType::Linear;
  tapBal.Linear().displayMultiplier = 100;
  tapBal.Linear().min = -1.0f;
  tapBal.Linear().max = 1.0f;
  auto selectTapBal = [](auto& module) { return &module.acc.tapBalance; };
  tapBal.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapBal);
  tapBal.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapBal);
  tapBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapBal);
  tapBal.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapBal);
  tapBal.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapBal);
  tapBal.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapXOver = result->params[(int)FFEchoParam::TapXOver];
  tapXOver.mode = FBParamMode::Accurate;
  tapXOver.unit = "%";
  tapXOver.defaultText = "0";
  tapXOver.name = "Tap L/R Crossover";
  tapXOver.matrixName = "Tap LR";
  tapXOver.display = "LRX";
  tapXOver.slotCount = FFEchoTapCount;
  tapXOver.id = prefix + "{C26420EA-2838-44E8-AA57-A4CA8E1A4759}";
  tapXOver.type = FBParamType::Identity;
  auto selectTapXOver = [](auto& module) { return &module.acc.tapXOver; };
  tapXOver.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapXOver);
  tapXOver.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapXOver);
  tapXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapXOver);
  tapXOver.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapXOver);
  tapXOver.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapXOver);
  tapXOver.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapLPOn = result->params[(int)FFEchoParam::TapLPOn];
  tapLPOn.mode = FBParamMode::Block;
  tapLPOn.name = "Tap LP On";
  tapLPOn.display = "LPF";
  tapLPOn.slotCount = FFEchoTapCount;
  tapLPOn.defaultText = "Off";
  tapLPOn.id = prefix + "{483FDB57-3842-49B8-8B66-4A642A926D16}";
  tapLPOn.type = FBParamType::Boolean;
  auto selectTapLPOn = [](auto& module) { return &module.block.tapLPOn; };
  tapLPOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapLPOn);
  tapLPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapLPOn);
  tapLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapLPOn);
  tapLPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapLPOn);
  tapLPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapLPOn);
  tapLPOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapLPFreq = result->params[(int)FFEchoParam::TapLPFreq];
  tapLPFreq.mode = FBParamMode::Accurate;
  tapLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  tapLPFreq.name = "Tap LPF Freq";
  tapLPFreq.display = "Frq";
  tapLPFreq.slotCount = FFEchoTapCount;
  tapLPFreq.unit = "Hz";
  tapLPFreq.id = prefix + "{D8CA3B61-6335-4457-9A4F-71F5A912C0D7}";
  tapLPFreq.type = FBParamType::Log2;
  tapLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectTapLPFreq = [](auto& module) { return &module.acc.tapLPFreq; };
  tapLPFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapLPFreq);
  tapLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapLPFreq);
  tapLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapLPFreq);
  tapLPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapLPFreq);
  tapLPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapLPFreq);
  tapLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::TapLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] != 0; });

  auto& tapLPRes = result->params[(int)FFEchoParam::TapLPRes];
  tapLPRes.mode = FBParamMode::Accurate;
  tapLPRes.defaultText = "0";
  tapLPRes.name = "Tap LPF Res";
  tapLPRes.display = "Res";
  tapLPRes.slotCount = FFEchoTapCount;
  tapLPRes.unit = "%";
  tapLPRes.id = prefix + "{38991C7F-01FB-459B-9086-1F19E876D3CA}";
  tapLPRes.type = FBParamType::Identity;
  auto selectTapLPRes = [](auto& module) { return &module.acc.tapLPRes; };
  tapLPRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapLPRes);
  tapLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapLPRes);
  tapLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapLPRes);
  tapLPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapLPRes);
  tapLPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapLPRes);
  tapLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::TapLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] != 0; });

  auto& tapHPOn = result->params[(int)FFEchoParam::TapHPOn];
  tapHPOn.mode = FBParamMode::Block;
  tapHPOn.name = "Tap HP On";
  tapHPOn.display = "HPF";
  tapHPOn.slotCount = FFEchoTapCount;
  tapHPOn.defaultText = "Off";
  tapHPOn.id = prefix + "{B60F324E-6A6A-4E02-AA9E-2C6345E26F6A}";
  tapHPOn.type = FBParamType::Boolean;
  auto selectTapHPOn = [](auto& module) { return &module.block.tapHPOn; };
  tapHPOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapHPOn);
  tapHPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapHPOn);
  tapHPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapHPOn);
  tapHPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapHPOn);
  tapHPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapHPOn);
  tapHPOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapHPFreq = result->params[(int)FFEchoParam::TapHPFreq];
  tapHPFreq.mode = FBParamMode::Accurate;
  tapHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  tapHPFreq.name = "Tap HPF Freq";
  tapHPFreq.display = "Frq";
  tapHPFreq.slotCount = FFEchoTapCount;
  tapHPFreq.unit = "Hz";
  tapHPFreq.id = prefix + "{3B3F90D7-CAF0-45ED-8316-D21BE4508E05}";
  tapHPFreq.type = FBParamType::Log2;
  tapHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectTapHPFreq = [](auto& module) { return &module.acc.tapHPFreq; };
  tapHPFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapHPFreq);
  tapHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapHPFreq);
  tapHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapHPFreq);
  tapHPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapHPFreq);
  tapHPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapHPFreq);
  tapHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::TapHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] != 0; });

  auto& tapHPRes = result->params[(int)FFEchoParam::TapHPRes];
  tapHPRes.mode = FBParamMode::Accurate;
  tapHPRes.defaultText = "0";
  tapHPRes.name = "Tap HPF Res";
  tapHPRes.display = "Res";
  tapHPRes.slotCount = FFEchoTapCount;
  tapHPRes.unit = "%";
  tapHPRes.id = prefix + "{E8174982-FF15-4375-84BA-1C8AFBE8CF28}";
  tapHPRes.type = FBParamType::Identity;
  auto selectTapHPRes = [](auto& module) { return &module.acc.tapHPRes; };
  tapHPRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapHPRes);
  tapHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapHPRes);
  tapHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapHPRes);
  tapHPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapHPRes);
  tapHPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapHPRes);
  tapHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::TapHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] != 0; });

  auto& feedbackOn = result->params[(int)FFEchoParam::FeedbackOn];
  feedbackOn.mode = FBParamMode::Block;
  feedbackOn.name = "Feedback On";
  feedbackOn.display = "Fdbk";
  feedbackOn.slotCount = 1;
  feedbackOn.defaultText = "On";
  feedbackOn.id = prefix + "{E28CD2D5-59B4-4C77-830F-BC961239299C}";
  feedbackOn.type = FBParamType::Boolean;
  auto selectFeedbackOn = [](auto& module) { return &module.block.feedbackOn; };
  feedbackOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackOn);
  feedbackOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackOn);
  feedbackOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackOn);
  feedbackOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackOn);
  feedbackOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackOn);
  feedbackOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& feedbackMix = result->params[(int)FFEchoParam::FeedbackMix];
  feedbackMix.mode = FBParamMode::Accurate;
  feedbackMix.unit = "%";
  feedbackMix.defaultText = "33";
  feedbackMix.name = "Feedback Mix";
  feedbackMix.matrixName = "Fdbk Mix";
  feedbackMix.display = "Mix";
  feedbackMix.slotCount = 1;
  feedbackMix.id = prefix + "{47D9FE5B-D161-4198-9AC2-44089966575E}";
  feedbackMix.type = FBParamType::Identity;
  auto selectFeedbackMix = [](auto& module) { return &module.acc.feedbackMix; };
  feedbackMix.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackMix);
  feedbackMix.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackMix);
  feedbackMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackMix);
  feedbackMix.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackMix);
  feedbackMix.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackMix);
  feedbackMix.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackXOver = result->params[(int)FFEchoParam::FeedbackXOver];
  feedbackXOver.unit = "%";
  feedbackXOver.mode = FBParamMode::Accurate;
  feedbackXOver.defaultText = "0";
  feedbackXOver.name = "Feedback L/R Crossover";
  feedbackXOver.matrixName = "Fdbk LR";
  feedbackXOver.display = "LRX";
  feedbackXOver.slotCount = 1;
  feedbackXOver.id = prefix + "{C4D67CA6-41FE-4B54-8C2B-9C87F0DA6F0C}";
  feedbackXOver.type = FBParamType::Identity;
  auto selectFeedbackXOver = [](auto& module) { return &module.acc.feedbackXOver; };
  feedbackXOver.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackXOver);
  feedbackXOver.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackXOver);
  feedbackXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackXOver);
  feedbackXOver.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackXOver);
  feedbackXOver.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackXOver);
  feedbackXOver.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackAmount = result->params[(int)FFEchoParam::FeedbackAmount];
  feedbackAmount.unit = "%";
  feedbackAmount.mode = FBParamMode::Accurate;
  feedbackAmount.defaultText = "67";
  feedbackAmount.name = "Feedback Amount";
  feedbackAmount.matrixName = "Fdbk Amt";
  feedbackAmount.display = "Amt";
  feedbackAmount.slotCount = 1;
  feedbackAmount.id = prefix + "{56017DC9-4B7D-44A5-AB7C-B6158BA584C3}";
  feedbackAmount.type = FBParamType::Identity;
  auto selectFeedbackAmount = [](auto& module) { return &module.acc.feedbackAmount; };
  feedbackAmount.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackAmount);
  feedbackAmount.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackAmount);
  feedbackAmount.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackAmount);
  feedbackAmount.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackAmount);
  feedbackAmount.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackAmount);
  feedbackAmount.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackDelayTime = result->params[(int)FFEchoParam::FeedbackDelayTime];
  feedbackDelayTime.mode = FBParamMode::Accurate;
  feedbackDelayTime.defaultText = "0.375";
  feedbackDelayTime.display = "Dly";
  feedbackDelayTime.name = "Feedback Delay Time";
  feedbackDelayTime.matrixName = "Fdbk Delay";
  feedbackDelayTime.slotCount = 1;
  feedbackDelayTime.unit = "Sec";
  feedbackDelayTime.id = prefix + "{CBD0A273-5E74-460E-A327-5EE9EE1C6F49}";
  feedbackDelayTime.type = FBParamType::Linear;
  feedbackDelayTime.Linear().min = 0.0f;
  feedbackDelayTime.Linear().max = 10.0f;
  feedbackDelayTime.Linear().editSkewFactor = 0.5f;
  auto selectFeedbackDelayTime = [](auto& module) { return &module.acc.feedbackDelayTime; };
  feedbackDelayTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackDelayTime);
  feedbackDelayTime.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackDelayTime);
  feedbackDelayTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackDelayTime);
  feedbackDelayTime.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackDelayTime);
  feedbackDelayTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackDelayTime);
  feedbackDelayTime.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  feedbackDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] == 0; });

  auto& feedbackDelayBars = result->params[(int)FFEchoParam::FeedbackDelayBars];
  feedbackDelayBars.mode = FBParamMode::Block;
  feedbackDelayBars.defaultText = "3/16";
  feedbackDelayBars.display = "Dly";
  feedbackDelayBars.name = "Feedback Delay Bars";
  feedbackDelayBars.matrixName = "Fdbk Delay Bars";
  feedbackDelayBars.slotCount = 1;
  feedbackDelayBars.unit = "Bars";
  feedbackDelayBars.id = prefix + "{BAC85A14-5F60-4692-9D45-81AB29477F61}";
  feedbackDelayBars.type = FBParamType::Bars;
  feedbackDelayBars.Bars().items = MakeEchoBarsItems();
  auto selectFeedbackDelayBars = [](auto& module) { return &module.block.feedbackDelayBars; };
  feedbackDelayBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackDelayBars);
  feedbackDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackDelayBars);
  feedbackDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackDelayBars);
  feedbackDelayBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackDelayBars);
  feedbackDelayBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackDelayBars);
  feedbackDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  feedbackDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });
  
  auto& feedbackLPOn = result->params[(int)FFEchoParam::FeedbackLPOn];
  feedbackLPOn.mode = FBParamMode::Block;
  feedbackLPOn.name = "Feedback LP On";
  feedbackLPOn.display = "LPF";
  feedbackLPOn.slotCount = 1;
  feedbackLPOn.defaultText = "Off";
  feedbackLPOn.id = prefix + "{CE69D29E-65D9-4B4B-AF76-16AAD3DC912F}";
  feedbackLPOn.type = FBParamType::Boolean;
  auto selectFeedbackLPOn = [](auto& module) { return &module.block.feedbackLPOn; };
  feedbackLPOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackLPOn);
  feedbackLPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackLPOn);
  feedbackLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackLPOn);
  feedbackLPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackLPOn);
  feedbackLPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackLPOn);
  feedbackLPOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackLPFreq = result->params[(int)FFEchoParam::FeedbackLPFreq];
  feedbackLPFreq.mode = FBParamMode::Accurate;
  feedbackLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  feedbackLPFreq.name = "Feedback LPF Freq";
  feedbackLPFreq.matrixName = "Fdbk LPF Freq";
  feedbackLPFreq.display = "Frq";
  feedbackLPFreq.slotCount = 1;
  feedbackLPFreq.unit = "Hz";
  feedbackLPFreq.id = prefix + "{E4D8EB30-BF2A-4935-AE14-053F6CF9C446}";
  feedbackLPFreq.type = FBParamType::Log2;
  feedbackLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectFeedbackLPFreq = [](auto& module) { return &module.acc.feedbackLPFreq; };
  feedbackLPFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackLPFreq);
  feedbackLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackLPFreq);
  feedbackLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackLPFreq);
  feedbackLPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackLPFreq);
  feedbackLPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackLPFreq);
  feedbackLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::FeedbackLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& feedbackLPRes = result->params[(int)FFEchoParam::FeedbackLPRes];
  feedbackLPRes.mode = FBParamMode::Accurate;
  feedbackLPRes.defaultText = "0";
  feedbackLPRes.name = "Feedback LPF Res";
  feedbackLPRes.matrixName = "Fdbk LPF Res";
  feedbackLPRes.display = "Res";
  feedbackLPRes.slotCount = 1;
  feedbackLPRes.unit = "%";
  feedbackLPRes.id = prefix + "{178C853C-F11C-45C6-A8D3-D62FFA202D9E}";
  feedbackLPRes.type = FBParamType::Identity;
  auto selectFeedbackLPRes = [](auto& module) { return &module.acc.feedbackLPRes; };
  feedbackLPRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackLPRes);
  feedbackLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackLPRes);
  feedbackLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackLPRes);
  feedbackLPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackLPRes);
  feedbackLPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackLPRes);
  feedbackLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::FeedbackLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& feedbackHPOn = result->params[(int)FFEchoParam::FeedbackHPOn];
  feedbackHPOn.mode = FBParamMode::Block;
  feedbackHPOn.name = "Feedback HP On";
  feedbackHPOn.display = "HPF";
  feedbackHPOn.slotCount = 1;
  feedbackHPOn.defaultText = "Off";
  feedbackHPOn.id = prefix + "{3A9398F7-38C2-4C3A-9F8A-33C1FBFE75D7}";
  feedbackHPOn.type = FBParamType::Boolean;
  auto selectFeedbackHPOn = [](auto& module) { return &module.block.feedbackHPOn; };
  feedbackHPOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackHPOn);
  feedbackHPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackHPOn);
  feedbackHPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackHPOn);
  feedbackHPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackHPOn);
  feedbackHPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackHPOn);
  feedbackHPOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });
  
  auto& feedbackHPFreq = result->params[(int)FFEchoParam::FeedbackHPFreq];
  feedbackHPFreq.mode = FBParamMode::Accurate;
  feedbackHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  feedbackHPFreq.name = "Feedback HPF Freq";
  feedbackHPFreq.matrixName = "Fdbk HPF Freq";
  feedbackHPFreq.display = "Frq";
  feedbackHPFreq.slotCount = 1;
  feedbackHPFreq.unit = "Hz";
  feedbackHPFreq.id = prefix + "{FE81A57D-1D98-4D3D-B563-8A63B099DB99}";
  feedbackHPFreq.type = FBParamType::Log2;
  feedbackHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectFeedbackHPFreq = [](auto& module) { return &module.acc.feedbackHPFreq; };
  feedbackHPFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackHPFreq);
  feedbackHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackHPFreq);
  feedbackHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackHPFreq);
  feedbackHPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackHPFreq);
  feedbackHPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackHPFreq);
  feedbackHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::FeedbackHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& feedbackHPRes = result->params[(int)FFEchoParam::FeedbackHPRes];
  feedbackHPRes.mode = FBParamMode::Accurate;
  feedbackHPRes.defaultText = "0";
  feedbackHPRes.name = "Feedback HPF Res";
  feedbackHPRes.matrixName = "Fdbk HPF Res";
  feedbackHPRes.display = "Res";
  feedbackHPRes.slotCount = 1;
  feedbackHPRes.unit = "%";
  feedbackHPRes.id = prefix + "{FD6F1412-EE09-4A50-835E-A8051AC39411}";
  feedbackHPRes.type = FBParamType::Identity;
  auto selectFeedbackHPRes = [](auto& module) { return &module.acc.feedbackHPRes; };
  feedbackHPRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackHPRes);
  feedbackHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackHPRes);
  feedbackHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackHPRes);
  feedbackHPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackHPRes);
  feedbackHPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackHPRes);
  feedbackHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::FeedbackHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& reverbOn = result->params[(int)FFEchoParam::ReverbOn];
  reverbOn.mode = FBParamMode::Block;
  reverbOn.name = "Reverb On";
  reverbOn.display = "Rvrb";
  reverbOn.slotCount = 1;
  reverbOn.defaultText = "Off";
  reverbOn.id = prefix + "{935D11F3-1131-40DE-81E5-FC3DCDCBAE73}";
  reverbOn.type = FBParamType::Boolean;
  auto selectReverbOn = [](auto& module) { return &module.block.reverbOn; };
  reverbOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbOn);
  reverbOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbOn);
  reverbOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbOn);
  reverbOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbOn);
  reverbOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbOn);
  reverbOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& reverbMix = result->params[(int)FFEchoParam::ReverbMix];
  reverbMix.mode = FBParamMode::Accurate;
  reverbMix.unit = "%";
  reverbMix.defaultText = "33";
  reverbMix.name = "Reverb Mix";
  reverbMix.matrixName = "Rvrb Mix";
  reverbMix.display = "Mix";
  reverbMix.slotCount = 1;
  reverbMix.id = prefix + "{D40CAAA2-186D-4296-BE87-7DFCCA33C4AF}";
  reverbMix.type = FBParamType::Identity;
  auto selectReverbMix = [](auto& module) { return &module.acc.reverbMix; };
  reverbMix.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbMix);
  reverbMix.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbMix);
  reverbMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbMix);
  reverbMix.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbMix);
  reverbMix.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbMix);
  reverbMix.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbXOver = result->params[(int)FFEchoParam::ReverbXOver];
  reverbXOver.unit = "%";
  reverbXOver.mode = FBParamMode::Accurate;
  reverbXOver.defaultText = "0";
  reverbXOver.name = "Reverb L/R Crossover";
  reverbXOver.matrixName = "Rvrb LR";
  reverbXOver.display = "LRX";
  reverbXOver.slotCount = 1;
  reverbXOver.id = prefix + "{F31A5809-1558-4B9C-A953-EAC108530E3E}";
  reverbXOver.type = FBParamType::Identity;
  auto selectReverbXOver = [](auto& module) { return &module.acc.reverbXOver; };
  reverbXOver.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbXOver);
  reverbXOver.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbXOver);
  reverbXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbXOver);
  reverbXOver.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbXOver);
  reverbXOver.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbXOver);
  reverbXOver.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbAPF = result->params[(int)FFEchoParam::ReverbAPF];
  reverbAPF.unit = "%";
  reverbAPF.mode = FBParamMode::Accurate;
  reverbAPF.defaultText = "50";
  reverbAPF.name = "Reverb APF";
  reverbAPF.matrixName = "Rvrb APF";
  reverbAPF.display = "APF";
  reverbAPF.slotCount = 1;
  reverbAPF.id = prefix + "{3D12C8EF-6891-4980-8BAD-33E74AD79298}";
  reverbAPF.type = FBParamType::Identity;
  auto selectReverbAPF = [](auto& module) { return &module.acc.reverbAPF; };
  reverbAPF.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbAPF);
  reverbAPF.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbAPF);
  reverbAPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbAPF);
  reverbAPF.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbAPF);
  reverbAPF.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbAPF);
  reverbAPF.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbDamp = result->params[(int)FFEchoParam::ReverbDamp];
  reverbDamp.unit = "%";
  reverbDamp.mode = FBParamMode::Accurate;
  reverbDamp.defaultText = "50";
  reverbDamp.name = "Reverb Damp";
  reverbDamp.matrixName = "Rvrb Damp";
  reverbDamp.display = "Damp";
  reverbDamp.slotCount = 1;
  reverbDamp.id = prefix + "{5E33AA4C-F65E-432A-9DCB-E67F4AF46770}";
  reverbDamp.type = FBParamType::Identity;
  auto selectReverbDamp = [](auto& module) { return &module.acc.reverbDamp; };
  reverbDamp.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbDamp);
  reverbDamp.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbDamp);
  reverbDamp.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbDamp);
  reverbDamp.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbDamp);
  reverbDamp.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbDamp);
  reverbDamp.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbSize = result->params[(int)FFEchoParam::ReverbSize];
  reverbSize.unit = "%";
  reverbSize.mode = FBParamMode::Accurate;
  reverbSize.defaultText = "50";
  reverbSize.name = "Reverb Size";
  reverbSize.matrixName = "Rvrb Size";
  reverbSize.display = "Size";
  reverbSize.slotCount = 1;
  reverbSize.id = prefix + "{2B4B1255-6875-49B3-B52D-CC9711C6D7A4}";
  reverbSize.type = FBParamType::Identity;
  auto selectReverbSize = [](auto& module) { return &module.acc.reverbSize; };
  reverbSize.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbSize);
  reverbSize.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbSize);
  reverbSize.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbSize);
  reverbSize.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbSize);
  reverbSize.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbSize);
  reverbSize.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbLPOn = result->params[(int)FFEchoParam::ReverbLPOn];
  reverbLPOn.mode = FBParamMode::Block;
  reverbLPOn.name = "Reverb LP On";
  reverbLPOn.display = "LPF";
  reverbLPOn.slotCount = 1;
  reverbLPOn.defaultText = "Off";
  reverbLPOn.id = prefix + "{BDB27A94-D5D0-44F6-8DBE-D98C70CFBD0D}";
  reverbLPOn.type = FBParamType::Boolean;
  auto selectReverbLPOn = [](auto& module) { return &module.block.reverbLPOn; };
  reverbLPOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbLPOn);
  reverbLPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbLPOn);
  reverbLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbLPOn);
  reverbLPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbLPOn);
  reverbLPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbLPOn);
  reverbLPOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbLPFreq = result->params[(int)FFEchoParam::ReverbLPFreq];
  reverbLPFreq.mode = FBParamMode::Accurate;
  reverbLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  reverbLPFreq.name = "Reverb LPF Freq";
  reverbLPFreq.matrixName = "Rvrb LPF Freq";
  reverbLPFreq.display = "Frq";
  reverbLPFreq.slotCount = 1;
  reverbLPFreq.unit = "Hz";
  reverbLPFreq.id = prefix + "{9B215206-8C8A-4F37-9F54-60EF7AAA67E1}";
  reverbLPFreq.type = FBParamType::Log2;
  reverbLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectReverbLPFreq = [](auto& module) { return &module.acc.reverbLPFreq; };
  reverbLPFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbLPFreq);
  reverbLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbLPFreq);
  reverbLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbLPFreq);
  reverbLPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbLPFreq);
  reverbLPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbLPFreq);
  reverbLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn, (int)FFEchoParam::ReverbLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& reverbLPRes = result->params[(int)FFEchoParam::ReverbLPRes];
  reverbLPRes.mode = FBParamMode::Accurate;
  reverbLPRes.defaultText = "0";
  reverbLPRes.name = "Reverb LPF Res";
  reverbLPRes.matrixName = "Rvrb LPF Res";
  reverbLPRes.display = "Res";
  reverbLPRes.slotCount = 1;
  reverbLPRes.unit = "%";
  reverbLPRes.id = prefix + "{94A61E63-8CEA-48B0-A2C1-AE170F8812D7}";
  reverbLPRes.type = FBParamType::Identity;
  auto selectReverbLPRes = [](auto& module) { return &module.acc.reverbLPRes; };
  reverbLPRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbLPRes);
  reverbLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbLPRes);
  reverbLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbLPRes);
  reverbLPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbLPRes);
  reverbLPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbLPRes);
  reverbLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn, (int)FFEchoParam::ReverbLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& reverbHPOn = result->params[(int)FFEchoParam::ReverbHPOn];
  reverbHPOn.mode = FBParamMode::Block;
  reverbHPOn.name = "Reverb HP On";
  reverbHPOn.display = "HPF";
  reverbHPOn.slotCount = 1;
  reverbHPOn.defaultText = "Off";
  reverbHPOn.id = prefix + "{D4C2AF4D-98C2-4A3B-9534-6C61FE2E5CA8}";
  reverbHPOn.type = FBParamType::Boolean;
  auto selectReverbHPOn = [](auto& module) { return &module.block.reverbHPOn; };
  reverbHPOn.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbHPOn);
  reverbHPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbHPOn);
  reverbHPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbHPOn);
  reverbHPOn.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbHPOn);
  reverbHPOn.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbHPOn);
  reverbHPOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbHPFreq = result->params[(int)FFEchoParam::ReverbHPFreq];
  reverbHPFreq.mode = FBParamMode::Accurate;
  reverbHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  reverbHPFreq.name = "Reverb HPF Freq";
  reverbHPFreq.matrixName = "Rvrb HPF Freq";
  reverbHPFreq.display = "Frq";
  reverbHPFreq.slotCount = 1;
  reverbHPFreq.unit = "Hz";
  reverbHPFreq.id = prefix + "{1CB4ACC6-1EA4-4693-B4D5-D93A73E1E01B}";
  reverbHPFreq.type = FBParamType::Log2;
  reverbHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectReverbHPFreq = [](auto& module) { return &module.acc.reverbHPFreq; };
  reverbHPFreq.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbHPFreq);
  reverbHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbHPFreq);
  reverbHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbHPFreq);
  reverbHPFreq.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbHPFreq);
  reverbHPFreq.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbHPFreq);
  reverbHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn, (int)FFEchoParam::ReverbHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& reverbHPRes = result->params[(int)FFEchoParam::ReverbHPRes];
  reverbHPRes.mode = FBParamMode::Accurate;
  reverbHPRes.defaultText = "0";
  reverbHPRes.name = "Reverb HPF Res";
  reverbHPRes.matrixName = "Rvrb HPF Res";
  reverbHPRes.display = "Res";
  reverbHPRes.slotCount = 1;
  reverbHPRes.unit = "%";
  reverbHPRes.id = prefix + "{AE3A50D6-187E-4C20-84DF-C9311103950C}";
  reverbHPRes.type = FBParamType::Identity;
  auto selectReverbHPRes = [](auto& module) { return &module.acc.reverbHPRes; };
  reverbHPRes.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectReverbHPRes);
  reverbHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbHPRes);
  reverbHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbHPRes);
  reverbHPRes.voiceAccProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectReverbHPRes);
  reverbHPRes.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectReverbHPRes);
  reverbHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VTargetOrGTarget, (int)FFEchoParam::ReverbOn, (int)FFEchoParam::ReverbHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });
  
  return result;
}