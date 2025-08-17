#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeGEchoBarsItems(bool withZero)
{
  return FBMakeBarsItems(withZero, { 1, 128 }, { 1, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeGEchoTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GEcho";
  result->slotCount = 1;
  result->id = "{B979D7BD-65A2-42E4-A7B2-3A48BBFFDE23}";
  result->params.resize((int)FFGEchoParam::Count);
  result->guiParams.resize((int)FFGEchoGUIParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gEcho; });
  auto selectGuiModule = [](auto& state) { return &state.gEcho; };
  auto selectModule = [](auto& state) { return &state.global.gEcho; };

  auto& target = result->params[(int)FFGEchoParam::Target];
  target.acc = false;
  target.defaultText = "Off";
  target.name = "Target";
  target.slotCount = 1;
  target.id = "{60F7D173-C5F9-46AD-A108-D17D40FE4C1D}";
  target.type = FBParamType::List;
  target.List().items = {
    { "{3B1FC348-4F80-4CD5-8F3D-E762C0373EAB}", "Off" },
    { "{84DBE1D7-B703-4E7A-BBDC-70A0F98EBB43}", "Voice" },
    { "{3F334789-DA27-442D-81CF-EF87C73067E5}", "GFX1" },
    { "{6888AC49-5381-4B1E-9E92-F78D49AB203A}", "GFX2" },
    { "{877E72D7-F9CA-441E-B675-800B4015E900}", "GFX3" },
    { "{C1ADFC32-1DD5-4CA8-B514-374E8306A2A1}", "GFX4" },
    { "{F0E3771C-D1FE-4544-BA9B-2D3C9C4E7CF4}", "Out" } };
  auto selectTarget = [](auto& module) { return &module.block.target; };
  target.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTarget);
  target.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTarget);
  target.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTarget);

  auto& sync = result->params[(int)FFGEchoParam::Sync];
  sync.acc = false;
  sync.name = "Tempo Sync";
  sync.display = "Sync";
  sync.slotCount = 1;
  sync.id = "{6CBE8845-0C59-4A95-B9F7-24C7AA061DFA}";
  sync.defaultText = "Off";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectScalarParamAddr(selectModule, selectSync);
  sync.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectSync);
  sync.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectSync);
  sync.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

  auto& mix = result->params[(int)FFGEchoParam::Mix];
  mix.acc = true;
  mix.unit = "%";
  mix.defaultText = "50";
  mix.name = "Mix";
  mix.slotCount = 1;
  mix.id = "{56DF16E6-A079-4E97-92D9-DA65826DB20F}";
  mix.type = FBParamType::Identity;
  auto selectMix = [](auto& module) { return &module.acc.mix; };
  mix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMix);
  mix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectMix);
  mix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMix);
  mix.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

  auto& guiTapSelect = result->guiParams[(int)FFGEchoGUIParam::TapSelect];
  guiTapSelect.defaultText = "1";
  guiTapSelect.name = "Tap Select";
  guiTapSelect.id = "{FB146F89-2B8D-448F-8B38-EA213B4FC84D}";
  guiTapSelect.slotCount = 1;
  guiTapSelect.type = FBParamType::Discrete;
  guiTapSelect.Discrete().valueOffset = 1;
  guiTapSelect.Discrete().valueCount = FFGEchoTapCount;
  auto selectGuiTapSelect = [](auto& module) { return &module.tapSelect; };
  guiTapSelect.scalarAddr = FFSelectGUIParamAddr(selectGuiModule, selectGuiTapSelect);
  guiTapSelect.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

  auto& tapOn = result->params[(int)FFGEchoParam::TapOn];
  tapOn.acc = false;
  tapOn.name = "Tap On";
  tapOn.display = "Tap";
  tapOn.slotCount = FFGEchoTapCount;
  tapOn.defaultText = "Off";
  tapOn.id = "{0EA26FE5-F45F-431E-9421-0FDD9E508CB8}";
  tapOn.type = FBParamType::Boolean;
  tapOn.slotFormatDisplay = true;
  auto selectTapOn = [](auto& module) { return &module.block.tapOn; };
  tapOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapOn);
  tapOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapOn);
  tapOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapOn);

  auto& tapLevel = result->params[(int)FFGEchoParam::TapLevel];
  tapLevel.acc = true;
  tapLevel.unit = "%";
  tapLevel.defaultText = "50";
  tapLevel.name = "Tap Level";
  tapLevel.display = "Level";
  tapLevel.slotCount = FFGEchoTapCount;
  tapLevel.id = "{E3633411-F79D-4D2A-A748-82E03A35434E}";
  tapLevel.type = FBParamType::Identity;
  auto selectTapLevel = [](auto& module) { return &module.acc.tapLevel; };
  tapLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLevel);
  tapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLevel);
  tapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLevel);

  auto& tapFeedback = result->params[(int)FFGEchoParam::TapFeedback];
  tapFeedback.acc = true;
  tapFeedback.unit = "%";
  tapFeedback.defaultText = "50";
  tapFeedback.name = "Tap Feedback";
  tapFeedback.matrixName = "Tap Fdbk";
  tapFeedback.display = "Fdbk";
  tapFeedback.slotCount = FFGEchoTapCount;
  tapFeedback.id = "{4E922DA7-ECAE-412E-AFED-BAA94205CF97}";
  tapFeedback.type = FBParamType::Identity;
  auto selectTapFeedback = [](auto& module) { return &module.acc.tapFeedback; };
  tapFeedback.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFeedback);
  tapFeedback.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapFeedback);
  tapFeedback.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFeedback);

  auto& tapBal = result->params[(int)FFGEchoParam::TapBalance];
  tapBal.acc = true;
  tapBal.defaultText = "0";
  tapBal.name = "Tap Balance";
  tapBal.matrixName = "Tap Bal";
  tapBal.display = "Bal";
  tapBal.slotCount = FFGEchoTapCount;
  tapBal.unit = "%";
  tapBal.id = "{2DCCEE86-4381-4E7F-98B6-FA94059F253B}";
  tapBal.type = FBParamType::Linear;
  tapBal.Linear().displayMultiplier = 100;
  tapBal.Linear().min = -1.0f;
  tapBal.Linear().max = 1.0f;
  auto selectTapBal = [](auto& module) { return &module.acc.tapBalance; };
  tapBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapBal);
  tapBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapBal);
  tapBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapBal);

  auto& tapXOver = result->params[(int)FFGEchoParam::TapXOver];
  tapXOver.acc = true;
  tapXOver.unit = "%";
  tapXOver.defaultText = "0";
  tapXOver.name = "Tap L/R Crossover";
  tapXOver.matrixName = "Tap XOver";
  tapXOver.display = "XOver";
  tapXOver.slotCount = FFGEchoTapCount;
  tapXOver.id = "{C26420EA-2838-44E8-AA57-A4CA8E1A4759}";
  tapXOver.type = FBParamType::Identity;
  auto selectTapXOver = [](auto& module) { return &module.acc.tapCrossOver; };
  tapXOver.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapXOver);
  tapXOver.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapXOver);
  tapXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapXOver);

  auto& tapFBXOver = result->params[(int)FFGEchoParam::TapFBXOver];
  tapFBXOver.acc = true;
  tapFBXOver.unit = "%";
  tapFBXOver.defaultText = "0";
  tapFBXOver.name = "Tap Feedback L/R Crossover";
  tapFBXOver.display = "FB XOver";
  tapFBXOver.matrixName = "Tap FB XOver";
  tapFBXOver.slotCount = FFGEchoTapCount;
  tapFBXOver.id = "{D60097D8-8F7A-4F05-9F35-65D41E2A4516}";
  tapFBXOver.type = FBParamType::Identity;
  auto selectTapFBXOver = [](auto& module) { return &module.acc.tapFBCrossOver; };
  tapFBXOver.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBXOver);
  tapFBXOver.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBXOver);
  tapFBXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBXOver);

  auto& tapLPOn = result->params[(int)FFGEchoParam::TapLPOn];
  tapLPOn.acc = false;
  tapLPOn.name = "Tap LPF On";
  tapLPOn.display = "LPF";
  tapLPOn.slotCount = FFGEchoTapCount;
  tapLPOn.defaultText = "Off";
  tapLPOn.id = "{F3400A25-4444-4865-823B-AA104A2D8ADB}";
  tapLPOn.type = FBParamType::Boolean;
  auto selectTapLPOn = [](auto& module) { return &module.block.tapLPOn; };
  tapLPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLPOn);
  tapLPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapLPOn);
  tapLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLPOn);

  auto& tapLPFreq = result->params[(int)FFGEchoParam::TapLPFreq];
  tapLPFreq.acc = true;
  tapLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  tapLPFreq.name = "Tap LPF Freq";
  tapLPFreq.display = "Frq";
  tapLPFreq.slotCount = FFGEchoTapCount;
  tapLPFreq.unit = "Hz";
  tapLPFreq.id = "{D8CA3B61-6335-4457-9A4F-71F5A912C0D7}";
  tapLPFreq.type = FBParamType::Log2;
  tapLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectTapLPFreq = [](auto& module) { return &module.acc.tapLPFreq; };
  tapLPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLPFreq);
  tapLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLPFreq);
  tapLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLPFreq);

  auto& tapLPRes = result->params[(int)FFGEchoParam::TapLPRes];
  tapLPRes.acc = true;
  tapLPRes.defaultText = "0";
  tapLPRes.name = "Tap LPF Res";
  tapLPRes.display = "Res";
  tapLPRes.slotCount = FFGEchoTapCount;
  tapLPRes.unit = "%";
  tapLPRes.id = "{38991C7F-01FB-459B-9086-1F19E876D3CA}";
  tapLPRes.type = FBParamType::Identity;
  auto selectTapLPRes = [](auto& module) { return &module.acc.tapLPRes; };
  tapLPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLPRes);
  tapLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLPRes);
  tapLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLPRes);

  auto& tapHPOn = result->params[(int)FFGEchoParam::TapHPOn];
  tapHPOn.acc = false;
  tapHPOn.name = "Tap HPF On";
  tapHPOn.display = "HPF";
  tapHPOn.slotCount = FFGEchoTapCount;
  tapHPOn.defaultText = "Off";
  tapHPOn.id = "{999DC4BF-C87C-471D-A251-29D2CDE9B7CD}";
  tapHPOn.type = FBParamType::Boolean;
  auto selectTapHPOn = [](auto& module) { return &module.block.tapHPOn; };
  tapHPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapHPOn);
  tapHPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapHPOn);
  tapHPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapHPOn);

  auto& tapHPFreq = result->params[(int)FFGEchoParam::TapHPFreq];
  tapHPFreq.acc = true;
  tapHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  tapHPFreq.name = "Tap HPF Freq";
  tapHPFreq.display = "Frq";
  tapHPFreq.slotCount = FFGEchoTapCount;
  tapHPFreq.unit = "Hz";
  tapHPFreq.id = "{3B3F90D7-CAF0-45ED-8316-D21BE4508E05}";
  tapHPFreq.type = FBParamType::Log2;
  tapHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectTapHPFreq = [](auto& module) { return &module.acc.tapHPFreq; };
  tapHPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapHPFreq);
  tapHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapHPFreq);
  tapHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapHPFreq);

  auto& tapHPRes = result->params[(int)FFGEchoParam::TapHPRes];
  tapHPRes.acc = true;
  tapHPRes.defaultText = "0";
  tapHPRes.name = "Tap HPF Res";
  tapHPRes.display = "Res";
  tapHPRes.slotCount = FFGEchoTapCount;
  tapHPRes.unit = "%";
  tapHPRes.id = "{E8174982-FF15-4375-84BA-1C8AFBE8CF28}";
  tapHPRes.type = FBParamType::Identity;
  auto selectTapHPRes = [](auto& module) { return &module.acc.tapHPRes; };
  tapHPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapHPRes);
  tapHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapHPRes);
  tapHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapHPRes);

  auto& tapFBLPOn = result->params[(int)FFGEchoParam::TapFBLPOn];
  tapFBLPOn.acc = false;
  tapFBLPOn.name = "Tap Feedback LPF On";
  tapFBLPOn.matrixName = "Tap FB LPF On";
  tapFBLPOn.display = "FB LPF";
  tapFBLPOn.slotCount = FFGEchoTapCount;
  tapFBLPOn.defaultText = "Off";
  tapFBLPOn.id = "{9EE6E317-8039-4E6B-BE6E-1B9F287D1F6E}";
  tapFBLPOn.type = FBParamType::Boolean;
  auto selectTapFBLPOn = [](auto& module) { return &module.block.tapFBLPOn; };
  tapFBLPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBLPOn);
  tapFBLPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBLPOn);
  tapFBLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBLPOn);

  auto& tapFBLPFreq = result->params[(int)FFGEchoParam::TapFBLPFreq];
  tapFBLPFreq.acc = true;
  tapFBLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  tapFBLPFreq.name = "Tap Feedback LPF Freq";
  tapFBLPFreq.matrixName = "Tap FB LPF Freq";
  tapFBLPFreq.display = "Frq";
  tapFBLPFreq.slotCount = FFGEchoTapCount;
  tapFBLPFreq.unit = "Hz";
  tapFBLPFreq.id = "{BD78678C-EDC1-4808-90A1-8969534170F4}";
  tapFBLPFreq.type = FBParamType::Log2;
  tapFBLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectTapFBLPFreq = [](auto& module) { return &module.acc.tapFBLPFreq; };
  tapFBLPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBLPFreq);
  tapFBLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBLPFreq);
  tapFBLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBLPFreq);

  auto& tapFBLPRes = result->params[(int)FFGEchoParam::TapFBLPRes];
  tapFBLPRes.acc = true;
  tapFBLPRes.defaultText = "0";
  tapFBLPRes.name = "Tap Feedback LPF Res";
  tapFBLPRes.matrixName = "Tap FB LPF Res";
  tapFBLPRes.display = "Res";
  tapFBLPRes.slotCount = FFGEchoTapCount;
  tapFBLPRes.unit = "%";
  tapFBLPRes.id = "{B9C9D5CA-8258-4A04-A172-524C0B1A072F}";
  tapFBLPRes.type = FBParamType::Identity;
  auto selectTapFBLPRes = [](auto& module) { return &module.acc.tapFBLPRes; };
  tapFBLPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBLPRes);
  tapFBLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBLPRes);
  tapFBLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBLPRes);

  auto& tapFBHPOn = result->params[(int)FFGEchoParam::TapFBHPOn];
  tapFBHPOn.acc = false;
  tapFBHPOn.name = "Tap Feedback HPF On";
  tapFBHPOn.display = "FB HPF";
  tapFBHPOn.slotCount = FFGEchoTapCount;
  tapFBHPOn.defaultText = "Off";
  tapFBHPOn.id = "{90188FE8-AE2E-4FD3-9185-44F976D350A5}";
  tapFBHPOn.type = FBParamType::Boolean;
  auto selectTapFBHPOn = [](auto& module) { return &module.block.tapFBHPOn; };
  tapFBHPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBHPOn);
  tapFBHPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBHPOn);
  tapFBHPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBHPOn);

  auto& tapFBHPFreq = result->params[(int)FFGEchoParam::TapFBHPFreq];
  tapFBHPFreq.acc = true;
  tapFBHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  tapFBHPFreq.name = "Tap Feedback HPF Freq";
  tapFBHPFreq.matrixName = "Tap FB HPF Freq";
  tapFBHPFreq.display = "Frq";
  tapFBHPFreq.slotCount = FFGEchoTapCount;
  tapFBHPFreq.unit = "Hz";
  tapFBHPFreq.id = "{18789F3C-57F6-45D6-BD6B-C2FD2763C998}";
  tapFBHPFreq.type = FBParamType::Log2;
  tapFBHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectTapFBHPFreq = [](auto& module) { return &module.acc.tapFBHPFreq; };
  tapFBHPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBHPFreq);
  tapFBHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBHPFreq);
  tapFBHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBHPFreq);

  auto& tapFBHPRes = result->params[(int)FFGEchoParam::TapFBHPRes];
  tapFBHPRes.acc = true;
  tapFBHPRes.defaultText = "0";
  tapFBHPRes.name = "Tap Feedback HPF Res";
  tapFBHPRes.matrixName = "Tap FB HPF Res";
  tapFBHPRes.display = "Res";
  tapFBHPRes.slotCount = FFGEchoTapCount;
  tapFBHPRes.unit = "%";
  tapFBHPRes.id = "{EE282F55-6537-4DAE-B8F9-DBD9F5C61811}";
  tapFBHPRes.type = FBParamType::Identity;
  auto selectTapFBHPRes = [](auto& module) { return &module.acc.tapFBHPRes; };
  tapFBHPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapFBHPRes);
  tapFBHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapFBHPRes);
  tapFBHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapFBHPRes);

  auto& tapDelayTime = result->params[(int)FFGEchoParam::TapDelayTime];
  tapDelayTime.acc = false;
  tapDelayTime.defaultText = "0";
  tapDelayTime.display = "Dly";
  tapDelayTime.name = "Tap Delay Time";
  tapDelayTime.slotCount = FFGEchoTapCount;
  tapDelayTime.unit = "Sec";
  tapDelayTime.id = "{7ADA9075-213B-4809-B32E-39B2794B010F}";
  tapDelayTime.type = FBParamType::Linear;
  tapDelayTime.Linear().min = 0.0f;
  tapDelayTime.Linear().max = 10.0f;
  tapDelayTime.Linear().editSkewFactor = 0.5f;
  auto selectTapDelayTime = [](auto& module) { return &module.block.tapDelayTime; };
  tapDelayTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapDelayTime);
  tapDelayTime.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapDelayTime);
  tapDelayTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapDelayTime);
  tapDelayTime.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  tapDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& tapDelayBars = result->params[(int)FFGEchoParam::TapDelayBars];
  tapDelayBars.acc = false;
  tapDelayBars.defaultText = "Off";
  tapDelayBars.display = "Dly";
  tapDelayBars.name = "Tap Delay Bars";
  tapDelayBars.slotCount = FFGEchoTapCount;
  tapDelayBars.unit = "Bars";
  tapDelayBars.id = "{BEDF76D3-211D-4A1F-AF42-85E9C4E5374F}";
  tapDelayBars.type = FBParamType::Bars;
  tapDelayBars.Bars().items = MakeGEchoBarsItems(true);
  auto selectTapDelayBars = [](auto& module) { return &module.block.tapDelayBars; };
  tapDelayBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapDelayBars);
  tapDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapDelayBars);
  tapDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapDelayBars);
  tapDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  tapDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& tapLengthTime = result->params[(int)FFGEchoParam::TapLengthTime];
  tapLengthTime.acc = true;
  tapLengthTime.defaultText = "1";
  tapLengthTime.display = "Len";
  tapLengthTime.name = "Tap Length Time";
  tapLengthTime.matrixName = "Tap Length";
  tapLengthTime.slotCount = FFGEchoTapCount;
  tapLengthTime.unit = "Sec";
  tapLengthTime.id = "{B18BA21F-3190-49A7-A584-C93EF76CC100}";
  tapLengthTime.type = FBParamType::Linear;
  tapLengthTime.Linear().min = 0.0f;
  tapLengthTime.Linear().max = 10.0f;
  tapLengthTime.Linear().editSkewFactor = 0.5f;
  auto selectTapLengthTime = [](auto& module) { return &module.acc.tapLengthTime; };
  tapLengthTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLengthTime);
  tapLengthTime.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLengthTime);
  tapLengthTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLengthTime);
  tapLengthTime.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  tapLengthTime.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& tapLengthBars = result->params[(int)FFGEchoParam::TapLengthBars];
  tapLengthBars.acc = false;
  tapLengthBars.defaultText = "1/4";
  tapLengthBars.display = "Len";
  tapLengthBars.name = "Tap Length Bars";
  tapLengthBars.slotCount = FFGEchoTapCount;
  tapLengthBars.unit = "Bars";
  tapLengthBars.id = "{785550ED-7885-449B-8F26-91BC4AE28B58}";
  tapLengthBars.type = FBParamType::Bars;
  tapLengthBars.Bars().items = MakeGEchoBarsItems(false);
  auto selectTapLengthBars = [](auto& module) { return &module.block.tapLengthBars; };
  tapLengthBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLengthBars);
  tapLengthBars.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapLengthBars);
  tapLengthBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLengthBars);
  tapLengthBars.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  tapLengthBars.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbPlacement = result->params[(int)FFGEchoParam::ReverbPlacement];
  reverbPlacement.acc = false;
  reverbPlacement.defaultText = "Off";
  reverbPlacement.name = "Reverb Placement";
  reverbPlacement.display = "Reverb";
  reverbPlacement.slotCount = 1;
  reverbPlacement.id = "{EC5479CD-0BD0-46D2-AD39-D6CE167FFE4C}";
  reverbPlacement.type = FBParamType::List;
  reverbPlacement.List().items = {
    { "{2063E052-591E-4F6A-8A72-8149779014BB}", "Off" },
    { "{9E345475-BE48-4D61-BD24-BC0B47E0E413}", "BeforeTaps" },
    { "{3DC9AB2F-A2E5-4344-A6B9-2D23E96190A9}", "AfterTaps" } };
  auto selectReverbPlacement = [](auto& module) { return &module.block.reverbPlacement; };
  reverbPlacement.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbPlacement);
  reverbPlacement.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReverbPlacement);
  reverbPlacement.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbPlacement);

  auto& reverbMix = result->params[(int)FFGEchoParam::ReverbMix];
  reverbMix.acc = true;
  reverbMix.unit = "%";
  reverbMix.defaultText = "50";
  reverbMix.name = "Reverb Mix";
  reverbMix.matrixName = "Rvrb Mix";
  reverbMix.display = "Mix";
  reverbMix.slotCount = 1;
  reverbMix.id = "{D40CAAA2-186D-4296-BE87-7DFCCA33C4AF}";
  reverbMix.type = FBParamType::Identity;
  auto selectReverbMix = [](auto& module) { return &module.acc.reverbMix; };
  reverbMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbMix);
  reverbMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbMix);
  reverbMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbMix);

  auto& reverbSpread = result->params[(int)FFGEchoParam::ReverbSpread];
  reverbSpread.unit = "%";
  reverbSpread.acc = true;
  reverbSpread.defaultText = "50";
  reverbSpread.name = "Reverb Spread";
  reverbSpread.matrixName = "Rvrb Spread";
  reverbSpread.display = "Sprd";
  reverbSpread.slotCount = 1;
  reverbSpread.id = "{F31A5809-1558-4B9C-A953-EAC108530E3E}";
  reverbSpread.type = FBParamType::Identity;
  auto selectReverbSpread = [](auto& module) { return &module.acc.reverbSpread; };
  reverbSpread.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbSpread);
  reverbSpread.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbSpread);
  reverbSpread.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbSpread);

  auto& reverbAPF = result->params[(int)FFGEchoParam::ReverbAPF];
  reverbAPF.unit = "%";
  reverbAPF.acc = true;
  reverbAPF.defaultText = "50";
  reverbAPF.name = "Reverb APF";
  reverbAPF.matrixName = "Rvrb APF";
  reverbAPF.display = "APF";
  reverbAPF.slotCount = 1;
  reverbAPF.id = "{3D12C8EF-6891-4980-8BAD-33E74AD79298}";
  reverbAPF.type = FBParamType::Identity;
  auto selectReverbAPF = [](auto& module) { return &module.acc.reverbAPF; };
  reverbAPF.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbAPF);
  reverbAPF.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbAPF);
  reverbAPF.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbAPF);

  auto& reverbDamp = result->params[(int)FFGEchoParam::ReverbDamp];
  reverbDamp.unit = "%";
  reverbDamp.acc = true;
  reverbDamp.defaultText = "50";
  reverbDamp.name = "Reverb Damp";
  reverbDamp.matrixName = "Rvrb Damp";
  reverbDamp.display = "Damp";
  reverbDamp.slotCount = 1;
  reverbDamp.id = "{5E33AA4C-F65E-432A-9DCB-E67F4AF46770}";
  reverbDamp.type = FBParamType::Identity;
  auto selectReverbDamp = [](auto& module) { return &module.acc.reverbDamp; };
  reverbDamp.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbDamp);
  reverbDamp.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbDamp);
  reverbDamp.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbDamp);

  auto& reverbSize = result->params[(int)FFGEchoParam::ReverbSize];
  reverbSize.unit = "%";
  reverbSize.acc = true;
  reverbSize.defaultText = "50";
  reverbSize.name = "Reverb Size";
  reverbSize.matrixName = "Rvrb Size";
  reverbSize.display = "Size";
  reverbSize.slotCount = 1;
  reverbSize.id = "{2B4B1255-6875-49B3-B52D-CC9711C6D7A4}";
  reverbSize.type = FBParamType::Identity;
  auto selectReverbSize = [](auto& module) { return &module.acc.reverbSize; };
  reverbSize.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbSize);
  reverbSize.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbSize);
  reverbSize.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbSize);

  auto& reverbLPOn = result->params[(int)FFGEchoParam::ReverbLPOn];
  reverbLPOn.acc = false;
  reverbLPOn.name = "Reverb LPF On";
  reverbLPOn.display = "LPF";
  reverbLPOn.slotCount = 1;
  reverbLPOn.defaultText = "Off";
  reverbLPOn.id = "{56AD0533-9A6E-403D-8648-15F46FBEF36A}";
  reverbLPOn.type = FBParamType::Boolean;
  auto selectReverbLPOn = [](auto& module) { return &module.block.reverbLPOn; };
  reverbLPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbLPOn);
  reverbLPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReverbLPOn);
  reverbLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbLPOn);

  auto& reverbLPFreq = result->params[(int)FFGEchoParam::ReverbLPFreq];
  reverbLPFreq.acc = true;
  reverbLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  reverbLPFreq.name = "Reverb LPF Freq";
  reverbLPFreq.matrixName = "Rvrb LPF Freq";
  reverbLPFreq.display = "Frq";
  reverbLPFreq.slotCount = 1;
  reverbLPFreq.unit = "Hz";
  reverbLPFreq.id = "{9B215206-8C8A-4F37-9F54-60EF7AAA67E1}";
  reverbLPFreq.type = FBParamType::Log2;
  reverbLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectReverbLPFreq = [](auto& module) { return &module.acc.reverbLPFreq; };
  reverbLPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbLPFreq);
  reverbLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbLPFreq);
  reverbLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbLPFreq);

  auto& reverbLPRes = result->params[(int)FFGEchoParam::ReverbLPRes];
  reverbLPRes.acc = true;
  reverbLPRes.defaultText = "0";
  reverbLPRes.name = "Reverb LPF Res";
  reverbLPRes.matrixName = "Rvrb LPF Res";
  reverbLPRes.display = "Res";
  reverbLPRes.slotCount = 1;
  reverbLPRes.unit = "%";
  reverbLPRes.id = "{94A61E63-8CEA-48B0-A2C1-AE170F8812D7}";
  reverbLPRes.type = FBParamType::Identity;
  auto selectReverbLPRes = [](auto& module) { return &module.acc.reverbLPRes; };
  reverbLPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbLPRes);
  reverbLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbLPRes);
  reverbLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbLPRes);

  auto& reverbHPOn = result->params[(int)FFGEchoParam::ReverbHPOn];
  reverbHPOn.acc = false;
  reverbHPOn.name = "Reverb HPF On";
  reverbHPOn.display = "HPF";
  reverbHPOn.slotCount = 1;
  reverbHPOn.defaultText = "Off";
  reverbHPOn.id = "{29C5F4A5-F769-46D4-BBA9-0880235D7E19}";
  reverbHPOn.type = FBParamType::Boolean;
  auto selectReverbHPOn = [](auto& module) { return &module.block.reverbHPOn; };
  reverbHPOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbHPOn);
  reverbHPOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReverbHPOn);
  reverbHPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbHPOn);

  auto& reverbHPFreq = result->params[(int)FFGEchoParam::ReverbHPFreq];
  reverbHPFreq.acc = true;
  reverbHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  reverbHPFreq.name = "Reverb HPF Freq";
  reverbHPFreq.matrixName = "Rvrb HPF Freq";
  reverbHPFreq.display = "Frq";
  reverbHPFreq.slotCount = 1;
  reverbHPFreq.unit = "Hz";
  reverbHPFreq.id = "{1CB4ACC6-1EA4-4693-B4D5-D93A73E1E01B}";
  reverbHPFreq.type = FBParamType::Log2;
  reverbHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectReverbHPFreq = [](auto& module) { return &module.acc.reverbHPFreq; };
  reverbHPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbHPFreq);
  reverbHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbHPFreq);
  reverbHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbHPFreq);

  auto& reverbHPRes = result->params[(int)FFGEchoParam::ReverbHPRes];
  reverbHPRes.acc = true;
  reverbHPRes.defaultText = "0";
  reverbHPRes.name = "Reverb HPF Res";
  reverbHPRes.matrixName = "Rvrb HPF Res";
  reverbHPRes.display = "Res";
  reverbHPRes.slotCount = 1;
  reverbHPRes.unit = "%";
  reverbHPRes.id = "{AE3A50D6-187E-4C20-84DF-C9311103950C}";
  reverbHPRes.type = FBParamType::Identity;
  auto selectReverbHPRes = [](auto& module) { return &module.acc.reverbHPRes; };
  reverbHPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbHPRes);
  reverbHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbHPRes);
  reverbHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbHPRes);

  return result;
}