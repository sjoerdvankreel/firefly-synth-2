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
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gEcho; });
  auto selectModule = [](auto& state) { return &state.global.gEcho; };

  auto& on = result->params[(int)FFGEchoParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.defaultText = "Off";
  on.id = "{5E624DFB-42F6-44B8-800D-B2382D8BC062}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOn);
  on.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOn);
  on.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOn);

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

  auto& mix = result->params[(int)FFGEchoParam::Mix];
  mix.acc = true;
  mix.defaultText = "50";
  mix.name = "Mix";
  mix.slotCount = 1;
  mix.id = "{56DF16E6-A079-4E97-92D9-DA65826DB20F}";
  mix.type = FBParamType::Identity;
  auto selectMix = [](auto& module) { return &module.acc.mix; };
  mix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectMix);
  mix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectMix);
  mix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectMix);

  auto& target = result->params[(int)FFGEchoParam::Target];
  target.acc = false;
  target.defaultText = "Out";
  target.name = "Target";
  target.slotCount = 1;
  target.id = "{60F7D173-C5F9-46AD-A108-D17D40FE4C1D}";
  target.type = FBParamType::List;
  target.List().items = {
    { "{84DBE1D7-B703-4E7A-BBDC-70A0F98EBB43}", "Voice" },
    { "{3F334789-DA27-442D-81CF-EF87C73067E5}", "FX1" },
    { "{6888AC49-5381-4B1E-9E92-F78D49AB203A}", "FX2" },
    { "{877E72D7-F9CA-441E-B675-800B4015E900}", "FX3" },
    { "{C1ADFC32-1DD5-4CA8-B514-374E8306A2A1}", "FX4" },
    { "{F0E3771C-D1FE-4544-BA9B-2D3C9C4E7CF4}", "Out" } };
  auto selectTarget = [](auto& module) { return &module.block.target; };
  target.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTarget);
  target.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTarget);
  target.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTarget);

  auto& tapOn = result->params[(int)FFGEchoParam::TapOn];
  tapOn.acc = false;
  tapOn.name = "Tap On";
  tapOn.display = "On";
  tapOn.slotCount = FFGEchoTapCount;
  tapOn.defaultText = "Off";
  tapOn.id = "{0EA26FE5-F45F-431E-9421-0FDD9E508CB8}";
  tapOn.type = FBParamType::Boolean;
  auto selectTapOn = [](auto& module) { return &module.block.tapOn; };
  tapOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapOn);
  tapOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapOn);
  tapOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapOn);

  auto& tapLevel = result->params[(int)FFGEchoParam::TapLevel];
  tapLevel.acc = true;
  tapLevel.defaultText = "50";
  tapLevel.name = "Tap Level";
  tapLevel.display = "Lvl";
  tapLevel.slotCount = FFGEchoTapCount;
  tapLevel.id = "{E3633411-F79D-4D2A-A748-82E03A35434E}";
  tapLevel.type = FBParamType::Identity;
  auto selectTapLevel = [](auto& module) { return &module.acc.tapLevel; };
  tapLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLevel);
  tapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLevel);
  tapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLevel);

  auto& tapFeedback = result->params[(int)FFGEchoParam::TapFeedback];
  tapFeedback.acc = true;
  tapFeedback.defaultText = "50";
  tapFeedback.name = "Tap Fdbk";
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
  tapBal.name = "Tap Bal";
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
  tapXOver.defaultText = "0";
  tapXOver.name = "Tap Crossover";
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
  tapFBXOver.defaultText = "0";
  tapFBXOver.name = "Tap Fdbk Crossover";
  tapFBXOver.display = "FB XOver";
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
  tapLPRes.name = "Tap LP Res";
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
  tapHPRes.name = "Tap HP Res";
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
  tapFBLPOn.name = "Tap Fdbk LPF On";
  tapFBLPOn.display = "Fdbk LPF";
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
  tapFBLPFreq.name = "Tap Fdbk LPF Freq";
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
  tapFBLPRes.name = "Tap Fdbk LP Res";
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
  tapFBHPOn.name = "Tap Fdbk HPF On";
  tapFBHPOn.display = "HPF";
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
  tapFBHPFreq.name = "Tap Fdbk HPF Freq";
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
  tapFBHPRes.name = "Tap Fdbk HP Res";
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

  auto& tapLengthTime = result->params[(int)FFGEchoParam::TapLengthTime];
  tapLengthTime.acc = true;
  tapLengthTime.defaultText = "1";
  tapLengthTime.display = "Len";
  tapLengthTime.name = "Tap Length Time";
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

  return result;
}