#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoGraph.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeEchoBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 1, 1 });
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
  result->voice = false;
  result->name = "GEcho";
  result->slotCount = 1;
  result->graphCount = 4;
  result->graphRenderer = FFGEchoRenderGraph;
  result->id = prefix + "{B979D7BD-65A2-42E4-A7B2-3A48BBFFDE23}";
  result->params.resize((int)FFGEchoParam::Count);
  result->guiParams.resize((int)FFEchoGUIParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gEcho; });
  auto selectGuiVoiceModule = [](auto& state) { return &state.vEcho; };
  auto selectGuiGlobalModule = [](auto& state) { return &state.gEcho; };
  auto selectVoiceModule = [](auto& state) { return &state.voice.vEcho; };
  auto selectGlobalModule = [](auto& state) { return &state.global.gEcho; };

  auto& vOnOrGTarget = result->params[(int)FFEchoParam::VOnOrGTarget];
  vOnOrGTarget.acc = false;
  vOnOrGTarget.slotCount = 1;
  vOnOrGTarget.id = prefix + "{60F7D173-C5F9-46AD-A108-D17D40FE4C1D}";
  auto selectVOnOrGTarget = [](auto& module) { return &module.block.vOnOrGTarget; };
  vOnOrGTarget.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVOnOrGTarget);
  if (global)
  {
    vOnOrGTarget.name = "Target";
    vOnOrGTarget.defaultText = "Off";
    vOnOrGTarget.type = FBParamType::List;
    vOnOrGTarget.List().items = {
      { "{90701058-1399-4D0A-B098-AE5AFFB9123C}", "Off" },
      { "{9A195809-A9A8-454A-ACD1-5376892EE416}", "Voice" },
      { "{38A69D55-AB3E-49F2-9CCD-44917FD24597}", "Out" },
      { "{CDE64B50-C65D-43E9-B52E-325F544E5175}", "GFX1 In" },
      { "{1C7AA2CC-2116-45E4-B7C2-AA9678CC15B5}", "GFX1 Out" },
      { "{68C1469F-96FF-47D7-8A2E-B24309E10031}", "GFX2 In" },
      { "{A043114B-C471-46A8-A526-B93E1897B1A0}", "GFX2 Out" },
      { "{883FA130-AB68-42D5-B56A-C61039DAD9A6}", "GFX3 In" },
      { "{56CE90EE-F9D0-4CCF-93B8-9BB0501DCBEF}", "GFX3 Out" },
      { "{80ECC9AA-AC6B-44DC-B305-43057720A4AC}", "GFX4 In" },
      { "{050E8D54-BFD2-4821-B14F-717D161D69DB}", "GFX4 Out" } };
    vOnOrGTarget.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVOnOrGTarget);
    vOnOrGTarget.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVOnOrGTarget);
  }
  else
  {
    auto& vOnOrGTarget = result->params[(int)FFGEchoParam::ReverbOn];
    vOnOrGTarget.name = "On";
    vOnOrGTarget.defaultText = "Off";
    vOnOrGTarget.type = FBParamType::Boolean;
    vOnOrGTarget.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVOnOrGTarget);
    vOnOrGTarget.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVOnOrGTarget);
  }

  auto& vOrderOrGOrder = result->params[(int)FFEchoParam::VOrderOrGOrder];
  vOrderOrGOrder.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });
  vOrderOrGOrder.acc = false;
  vOrderOrGOrder.slotCount = 1;
  vOrderOrGOrder.name = "Order";
  vOrderOrGOrder.id = prefix + "{D84A623A-7F5C-40C6-A892-A441497783CD}";
  auto selectVOrderOrGOrder = [](auto& module) { return &module.block.vOrderOrGOrder; };
  vOrderOrGOrder.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectVOrderOrGOrder);
  if (global)
  {
    vOrderOrGOrder.defaultText = "T\U00002192F\U00002192R";
    vOrderOrGOrder.type = FBParamType::List;
    vOrderOrGOrder.List().items = {
      { "{90701058-1399-4D0A-B098-AE5AFFB9123C}", "T\U00002192F\U00002192R" },
      { "{9A195809-A9A8-454A-ACD1-5376892EE416}", "T\U00002192R\U00002192F" },
      { "{883FA130-AB68-42D5-B56A-C61039DAD9A6}", "F\U00002192T\U00002192R" },
      { "{56CE90EE-F9D0-4CCF-93B8-9BB0501DCBEF}", "F\U00002192R\U00002192T" },
      { "{80ECC9AA-AC6B-44DC-B305-43057720A4AC}", "R\U00002192T\U00002192F" },
      { "{050E8D54-BFD2-4821-B14F-717D161D69DB}", "R\U00002192F\U00002192T" } };
    vOrderOrGOrder.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectVOrderOrGOrder);
    vOrderOrGOrder.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectVOrderOrGOrder);
  }
  else
  {
    vOrderOrGOrder.defaultText = "T\U00002192F";
    vOrderOrGOrder.type = FBParamType::List;
    vOrderOrGOrder.List().items = {
      { "{25C92192-05D9-470A-BBF9-DBD6B6B2D666}", "T\U00002192F" },
      { "{31F6E72F-4F26-48EC-B292-044DD5F1A97F}", "F\U00002192T" } };
    vOrderOrGOrder.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectVOrderOrGOrder);
    vOrderOrGOrder.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectVOrderOrGOrder);
  }

  auto& gain = result->params[(int)FFEchoParam::Gain];
  gain.acc = true;
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
  gain.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& sync = result->params[(int)FFEchoParam::Sync];
  sync.acc = false;
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
  sync.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& delaySmoothTime = result->params[(int)FFEchoParam::DelaySmoothTime];
  delaySmoothTime.acc = false;
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
  delaySmoothTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& delaySmoothBars = result->params[(int)FFEchoParam::DelaySmoothBars];
  delaySmoothBars.acc = false;
  delaySmoothBars.display = "Smth";
  delaySmoothBars.name = "Delay Smooth Bars";
  delaySmoothBars.slotCount = 1;
  delaySmoothBars.unit = "Bars";
  delaySmoothBars.id = prefix + "{5554CC4B-29E6-4F3C-8FFE-95947033676C}";
  delaySmoothBars.defaultText = "1/4";
  delaySmoothBars.type = FBParamType::Bars;
  delaySmoothBars.Bars().items = MakeGEchoSmoothBarsItems();
  auto selectDelaySmoothBars = [](auto& module) { return &module.block.delaySmoothBars; };
  delaySmoothBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectDelaySmoothBars);
  delaySmoothBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectDelaySmoothBars);
  delaySmoothBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectDelaySmoothBars);
  delaySmoothBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectDelaySmoothBars);
  delaySmoothBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectDelaySmoothBars);
  delaySmoothBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  delaySmoothBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& tapsMix = result->params[(int)FFEchoParam::TapsMix];
  tapsMix.acc = true;
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
  tapsMix.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& tapsOn = result->params[(int)FFEchoParam::TapsOn];
  tapsOn.acc = false;
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
  tapsOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& guiTapSelect = result->guiParams[(int)FFEchoGUIParam::TapSelect];
  guiTapSelect.defaultText = "1";
  guiTapSelect.name = "Select";
  guiTapSelect.id = prefix + "{FB146F89-2B8D-448F-8B38-EA213B4FC84D}";
  guiTapSelect.slotCount = 1;
  guiTapSelect.type = FBParamType::Discrete;
  guiTapSelect.Discrete().valueOffset = 1;
  guiTapSelect.Discrete().valueCount = FFEchoTapCount;
  auto selectGuiTapSelect = [](auto& module) { return &module.tapSelect; };
  guiTapSelect.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiTapSelect);
  guiTapSelect.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& tapOn = result->params[(int)FFEchoParam::TapOn];
  tapOn.acc = false;
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
  tapOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& tapLevel = result->params[(int)FFEchoParam::TapLevel];
  tapLevel.acc = true;
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
  tapLevel.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });
    
  auto& tapDelayTime = result->params[(int)FFEchoParam::TapDelayTime];
  tapDelayTime.acc = true;
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
  tapDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] == 0; });

  auto& tapDelayBars = result->params[(int)FFEchoParam::TapDelayBars];
  tapDelayBars.acc = false;
  tapDelayBars.display = "Dly";
  tapDelayBars.name = "Tap Delay Bars";
  tapDelayBars.slotCount = FFEchoTapCount;
  tapDelayBars.unit = "Bars";
  tapDelayBars.id = prefix + "{BEDF76D3-211D-4A1F-AF42-85E9C4E5374F}";
  tapDelayBars.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "1/4" : ps == 1 ? "1/2" : ps == 2 ? "3/4" : ps == 3 ? "1/1" : "Off"; };
  tapDelayBars.type = FBParamType::Bars;
  tapDelayBars.Bars().items = MakeGEchoBarsItems();
  auto selectTapDelayBars = [](auto& module) { return &module.block.tapDelayBars; };
  tapDelayBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectTapDelayBars);
  tapDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectTapDelayBars);
  tapDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectTapDelayBars);
  tapDelayBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectTapDelayBars);
  tapDelayBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectTapDelayBars);
  tapDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  tapDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0 && vs[3] != 0; });
   
  auto& tapBal = result->params[(int)FFEchoParam::TapBalance];
  tapBal.acc = true;
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
  tapBal.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapXOver = result->params[(int)FFEchoParam::TapXOver];
  tapXOver.acc = true;
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
  tapXOver.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapLPFreq = result->params[(int)FFEchoParam::TapLPFreq];
  tapLPFreq.acc = true;
  tapLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  tapLPFreq.name = "Tap LPF Freq";
  tapLPFreq.display = "LPF";
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
  tapLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapLPRes = result->params[(int)FFEchoParam::TapLPRes];
  tapLPRes.acc = true;
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
  tapLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapHPFreq = result->params[(int)FFEchoParam::TapHPFreq];
  tapHPFreq.acc = true;
  tapHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  tapHPFreq.name = "Tap HPF Freq";
  tapHPFreq.display = "HPF";
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
  tapHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapHPRes = result->params[(int)FFEchoParam::TapHPRes];
  tapHPRes.acc = true;
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
  tapHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::TapsOn, (int)FFEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& feedbackOn = result->params[(int)FFEchoParam::FeedbackOn];
  feedbackOn.acc = false;
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
  feedbackOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& feedbackMix = result->params[(int)FFEchoParam::FeedbackMix];
  feedbackMix.acc = true;
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
  feedbackMix.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackXOver = result->params[(int)FFEchoParam::FeedbackXOver];
  feedbackXOver.unit = "%";
  feedbackXOver.acc = true;
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
  feedbackXOver.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackAmount = result->params[(int)FFEchoParam::FeedbackAmount];
  feedbackAmount.unit = "%";
  feedbackAmount.acc = true;
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
  feedbackAmount.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackDelayTime = result->params[(int)FFEchoParam::FeedbackDelayTime];
  feedbackDelayTime.acc = true;
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
  feedbackDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] == 0; });

  auto& feedbackDelayBars = result->params[(int)FFEchoParam::FeedbackDelayBars];
  feedbackDelayBars.acc = false;
  feedbackDelayBars.defaultText = "3/16";
  feedbackDelayBars.display = "Dly";
  feedbackDelayBars.name = "Feedback Delay Bars";
  feedbackDelayBars.matrixName = "Fdbk Delay Bars";
  feedbackDelayBars.slotCount = 1;
  feedbackDelayBars.unit = "Bars";
  feedbackDelayBars.id = prefix + "{BAC85A14-5F60-4692-9D45-81AB29477F61}";
  feedbackDelayBars.type = FBParamType::Bars;
  feedbackDelayBars.Bars().items = MakeGEchoBarsItems();
  auto selectFeedbackDelayBars = [](auto& module) { return &module.block.feedbackDelayBars; };
  feedbackDelayBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectFeedbackDelayBars);
  feedbackDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectFeedbackDelayBars);
  feedbackDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectFeedbackDelayBars);
  feedbackDelayBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectFeedbackDelayBars);
  feedbackDelayBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectFeedbackDelayBars);
  feedbackDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  feedbackDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn, (int)FFEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });
  
  auto& feedbackLPFreq = result->params[(int)FFEchoParam::FeedbackLPFreq];
  feedbackLPFreq.acc = true;
  feedbackLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  feedbackLPFreq.name = "Feedback LPF Freq";
  feedbackLPFreq.matrixName = "Fdbk LPF Freq";
  feedbackLPFreq.display = "LPF";
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
  feedbackLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackLPRes = result->params[(int)FFEchoParam::FeedbackLPRes];
  feedbackLPRes.acc = true;
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
  feedbackLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackHPFreq = result->params[(int)FFEchoParam::FeedbackHPFreq];
  feedbackHPFreq.acc = true;
  feedbackHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  feedbackHPFreq.name = "Feedback HPF Freq";
  feedbackHPFreq.matrixName = "Fdbk HPF Freq";
  feedbackHPFreq.display = "HPF";
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
  feedbackHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackHPRes = result->params[(int)FFEchoParam::FeedbackHPRes];
  feedbackHPRes.acc = true;
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
  feedbackHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  // No reverb for per-voice.
  if (!global)
    return result;

  auto& reverbOn = result->params[(int)FFGEchoParam::ReverbOn];
  reverbOn.acc = false;
  reverbOn.name = "Reverb On";
  reverbOn.display = "Rvrb";
  reverbOn.slotCount = 1;
  reverbOn.defaultText = "Off";
  reverbOn.id = prefix + "{935D11F3-1131-40DE-81E5-FC3DCDCBAE73}";
  reverbOn.type = FBParamType::Boolean;
  auto selectReverbOn = [](auto& module) { return &module.block.reverbOn; };
  reverbOn.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbOn);
  reverbOn.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbOn);
  reverbOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbOn);
  reverbOn.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget }, [](auto const& vs) { return vs[0] != 0; });

  auto& reverbMix = result->params[(int)FFGEchoParam::ReverbMix];
  reverbMix.acc = true;
  reverbMix.unit = "%";
  reverbMix.defaultText = "33";
  reverbMix.name = "Reverb Mix";
  reverbMix.matrixName = "Rvrb Mix";
  reverbMix.display = "Mix";
  reverbMix.slotCount = 1;
  reverbMix.id = prefix + "{D40CAAA2-186D-4296-BE87-7DFCCA33C4AF}";
  reverbMix.type = FBParamType::Identity;
  auto selectReverbMix = [](auto& module) { return &module.acc.reverbMix; };
  reverbMix.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbMix);
  reverbMix.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbMix);
  reverbMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbMix);
  reverbMix.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbXOver = result->params[(int)FFGEchoParam::ReverbXOver];
  reverbXOver.unit = "%";
  reverbXOver.acc = true;
  reverbXOver.defaultText = "0";
  reverbXOver.name = "Reverb L/R Crossover";
  reverbXOver.matrixName = "Rvrb LR";
  reverbXOver.display = "LRX";
  reverbXOver.slotCount = 1;
  reverbXOver.id = prefix + "{F31A5809-1558-4B9C-A953-EAC108530E3E}";
  reverbXOver.type = FBParamType::Identity;
  auto selectReverbXOver = [](auto& module) { return &module.acc.reverbXOver; };
  reverbXOver.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbXOver);
  reverbXOver.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbXOver);
  reverbXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbXOver);
  reverbXOver.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbAPF = result->params[(int)FFGEchoParam::ReverbAPF];
  reverbAPF.unit = "%";
  reverbAPF.acc = true;
  reverbAPF.defaultText = "50";
  reverbAPF.name = "Reverb APF";
  reverbAPF.matrixName = "Rvrb APF";
  reverbAPF.display = "APF";
  reverbAPF.slotCount = 1;
  reverbAPF.id = prefix + "{3D12C8EF-6891-4980-8BAD-33E74AD79298}";
  reverbAPF.type = FBParamType::Identity;
  auto selectReverbAPF = [](auto& module) { return &module.acc.reverbAPF; };
  reverbAPF.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbAPF);
  reverbAPF.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbAPF);
  reverbAPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbAPF);
  reverbAPF.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbDamp = result->params[(int)FFGEchoParam::ReverbDamp];
  reverbDamp.unit = "%";
  reverbDamp.acc = true;
  reverbDamp.defaultText = "50";
  reverbDamp.name = "Reverb Damp";
  reverbDamp.matrixName = "Rvrb Damp";
  reverbDamp.display = "Damp";
  reverbDamp.slotCount = 1;
  reverbDamp.id = prefix + "{5E33AA4C-F65E-432A-9DCB-E67F4AF46770}";
  reverbDamp.type = FBParamType::Identity;
  auto selectReverbDamp = [](auto& module) { return &module.acc.reverbDamp; };
  reverbDamp.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbDamp);
  reverbDamp.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbDamp);
  reverbDamp.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbDamp);
  reverbDamp.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbSize = result->params[(int)FFGEchoParam::ReverbSize];
  reverbSize.unit = "%";
  reverbSize.acc = true;
  reverbSize.defaultText = "50";
  reverbSize.name = "Reverb Size";
  reverbSize.matrixName = "Rvrb Size";
  reverbSize.display = "Size";
  reverbSize.slotCount = 1;
  reverbSize.id = prefix + "{2B4B1255-6875-49B3-B52D-CC9711C6D7A4}";
  reverbSize.type = FBParamType::Identity;
  auto selectReverbSize = [](auto& module) { return &module.acc.reverbSize; };
  reverbSize.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbSize);
  reverbSize.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbSize);
  reverbSize.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbSize);
  reverbSize.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbLPFreq = result->params[(int)FFGEchoParam::ReverbLPFreq];
  reverbLPFreq.acc = true;
  reverbLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  reverbLPFreq.name = "Reverb LPF Freq";
  reverbLPFreq.matrixName = "Rvrb LPF Freq";
  reverbLPFreq.display = "LPF";
  reverbLPFreq.slotCount = 1;
  reverbLPFreq.unit = "Hz";
  reverbLPFreq.id = prefix + "{9B215206-8C8A-4F37-9F54-60EF7AAA67E1}";
  reverbLPFreq.type = FBParamType::Log2;
  reverbLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectReverbLPFreq = [](auto& module) { return &module.acc.reverbLPFreq; };
  reverbLPFreq.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbLPFreq);
  reverbLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbLPFreq);
  reverbLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbLPFreq);
  reverbLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbLPRes = result->params[(int)FFGEchoParam::ReverbLPRes];
  reverbLPRes.acc = true;
  reverbLPRes.defaultText = "0";
  reverbLPRes.name = "Reverb LPF Res";
  reverbLPRes.matrixName = "Rvrb LPF Res";
  reverbLPRes.display = "Res";
  reverbLPRes.slotCount = 1;
  reverbLPRes.unit = "%";
  reverbLPRes.id = prefix + "{94A61E63-8CEA-48B0-A2C1-AE170F8812D7}";
  reverbLPRes.type = FBParamType::Identity;
  auto selectReverbLPRes = [](auto& module) { return &module.acc.reverbLPRes; };
  reverbLPRes.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbLPRes);
  reverbLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbLPRes);
  reverbLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbLPRes);
  reverbLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbHPFreq = result->params[(int)FFGEchoParam::ReverbHPFreq];
  reverbHPFreq.acc = true;
  reverbHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  reverbHPFreq.name = "Reverb HPF Freq";
  reverbHPFreq.matrixName = "Rvrb HPF Freq";
  reverbHPFreq.display = "HPF";
  reverbHPFreq.slotCount = 1;
  reverbHPFreq.unit = "Hz";
  reverbHPFreq.id = prefix + "{1CB4ACC6-1EA4-4693-B4D5-D93A73E1E01B}";
  reverbHPFreq.type = FBParamType::Log2;
  reverbHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectReverbHPFreq = [](auto& module) { return &module.acc.reverbHPFreq; };
  reverbHPFreq.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbHPFreq);
  reverbHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbHPFreq);
  reverbHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbHPFreq);
  reverbHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbHPRes = result->params[(int)FFGEchoParam::ReverbHPRes];
  reverbHPRes.acc = true;
  reverbHPRes.defaultText = "0";
  reverbHPRes.name = "Reverb HPF Res";
  reverbHPRes.matrixName = "Rvrb HPF Res";
  reverbHPRes.display = "Res";
  reverbHPRes.slotCount = 1;
  reverbHPRes.unit = "%";
  reverbHPRes.id = prefix + "{AE3A50D6-187E-4C20-84DF-C9311103950C}";
  reverbHPRes.type = FBParamType::Identity;
  auto selectReverbHPRes = [](auto& module) { return &module.acc.reverbHPRes; };
  reverbHPRes.scalarAddr = FFSelectScalarParamAddr(selectGlobalModule, selectReverbHPRes);
  reverbHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectReverbHPRes);
  reverbHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectReverbHPRes);
  reverbHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFEchoParam::VOnOrGTarget, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });
  
  return result;
}