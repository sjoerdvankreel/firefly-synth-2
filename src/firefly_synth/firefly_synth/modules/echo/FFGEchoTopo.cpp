#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeGEchoBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 1, 1 });
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
  auto selectTarget = [](auto& module) { return &module.block.target; };
  target.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTarget);
  target.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTarget);
  target.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTarget);

  auto& order = result->params[(int)FFGEchoParam::Order];
  order.acc = false;
  order.defaultText = "T\U00002192F\U00002192R";
  order.name = "Order";
  order.slotCount = 1;
  order.id = "{D84A623A-7F5C-40C6-A892-A441497783CD}";
  order.type = FBParamType::List;
  order.List().items = {
    { "{90701058-1399-4D0A-B098-AE5AFFB9123C}", "T\U00002192F\U00002192R" },
    { "{9A195809-A9A8-454A-ACD1-5376892EE416}", "T\U00002192R\U00002192F" },
    { "{883FA130-AB68-42D5-B56A-C61039DAD9A6}", "F\U00002192T\U00002192R" },
    { "{56CE90EE-F9D0-4CCF-93B8-9BB0501DCBEF}", "F\U00002192R\U00002192T" },
    { "{80ECC9AA-AC6B-44DC-B305-43057720A4AC}", "R\U00002192T\U00002192F" },
    { "{050E8D54-BFD2-4821-B14F-717D161D69DB}", "R\U00002192F\U00002192T" } };
  auto selectOrder = [](auto& module) { return &module.block.order; };
  order.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOrder);
  order.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOrder);
  order.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOrder);
  order.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

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

  auto& tapsMix = result->params[(int)FFGEchoParam::TapsMix];
  tapsMix.acc = true;
  tapsMix.unit = "%";
  tapsMix.defaultText = "33";
  tapsMix.name = "Taps Mix";
  tapsMix.slotCount = 1;
  tapsMix.id = "{56DF16E6-A079-4E97-92D9-DA65826DB20F}";
  tapsMix.type = FBParamType::Identity;
  auto selectTapsMix = [](auto& module) { return &module.acc.tapsMix; };
  tapsMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapsMix);
  tapsMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapsMix);
  tapsMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapsMix);
  tapsMix.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

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
  tapOn.defaultTextSelector = [](int, int, int ps) { return ps < 4 ? "On" : "Off"; };
  tapOn.id = "{0EA26FE5-F45F-431E-9421-0FDD9E508CB8}";
  tapOn.type = FBParamType::Boolean;
  tapOn.slotFormatDisplay = true;
  auto selectTapOn = [](auto& module) { return &module.block.tapOn; };
  tapOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapOn);
  tapOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapOn);
  tapOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapOn);
  tapOn.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

  auto& tapLevel = result->params[(int)FFGEchoParam::TapLevel];
  tapLevel.acc = true;
  tapLevel.unit = "%";
  tapLevel.name = "Tap Level";
  tapLevel.display = "Level";
  tapLevel.slotCount = FFGEchoTapCount;
  tapLevel.id = "{E3633411-F79D-4D2A-A748-82E03A35434E}";
  tapLevel.defaultTextSelector = [](int, int, int ps) { return ps == 0? "100": ps == 1? "80": ps == 2? "60": ps == 3? "40": "0"; };
  tapLevel.type = FBParamType::Identity;
  auto selectTapLevel = [](auto& module) { return &module.acc.tapLevel; };
  tapLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLevel);
  tapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLevel);
  tapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLevel);
  tapLevel.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });
    
  auto& tapDelayTime = result->params[(int)FFGEchoParam::TapDelayTime];
  tapDelayTime.acc = true;
  tapDelayTime.display = "Dly";
  tapDelayTime.name = "Tap Delay Time";
  tapDelayTime.slotCount = FFGEchoTapCount;
  tapDelayTime.unit = "Sec";
  tapDelayTime.id = "{7ADA9075-213B-4809-B32E-39B2794B010F}";
  tapDelayTime.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "0.5" : ps == 1 ? "1" : ps == 2 ? "1.5" : ps == 3 ? "2" : "0"; };
  tapDelayTime.type = FBParamType::Linear;
  tapDelayTime.Linear().min = 0.0f;
  tapDelayTime.Linear().max = 10.0f;
  tapDelayTime.Linear().editSkewFactor = 0.5f;
  auto selectTapDelayTime = [](auto& module) { return &module.acc.tapDelayTime; };
  tapDelayTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapDelayTime);
  tapDelayTime.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapDelayTime);
  tapDelayTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapDelayTime);
  tapDelayTime.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  tapDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] == 0; });

  auto& tapDelayBars = result->params[(int)FFGEchoParam::TapDelayBars];
  tapDelayBars.acc = false;
  tapDelayBars.display = "Dly";
  tapDelayBars.name = "Tap Delay Bars";
  tapDelayBars.slotCount = FFGEchoTapCount;
  tapDelayBars.unit = "Bars";
  tapDelayBars.id = "{BEDF76D3-211D-4A1F-AF42-85E9C4E5374F}";
  tapDelayBars.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "1/8" : ps == 1 ? "1/4" : ps == 2 ? "3/8" : ps == 3 ? "1/2" : "Off"; };
  tapDelayBars.type = FBParamType::Bars;
  tapDelayBars.Bars().items = MakeGEchoBarsItems();
  auto selectTapDelayBars = [](auto& module) { return &module.block.tapDelayBars; };
  tapDelayBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapDelayBars);
  tapDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectTapDelayBars);
  tapDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapDelayBars);
  tapDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  tapDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& tapBal = result->params[(int)FFGEchoParam::TapBalance];
  tapBal.acc = true;
  tapBal.name = "Tap Balance";
  tapBal.matrixName = "Tap Bal";
  tapBal.display = "Bal";
  tapBal.slotCount = FFGEchoTapCount;
  tapBal.unit = "%";
  tapBal.id = "{2DCCEE86-4381-4E7F-98B6-FA94059F253B}";
  tapBal.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "-33" : ps == 1 ? "33" : ps == 2 ? "-67" : ps == 3 ? "67" : "0"; };
  tapBal.type = FBParamType::Linear;
  tapBal.Linear().displayMultiplier = 100;
  tapBal.Linear().min = -1.0f;
  tapBal.Linear().max = 1.0f;
  auto selectTapBal = [](auto& module) { return &module.acc.tapBalance; };
  tapBal.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapBal);
  tapBal.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapBal);
  tapBal.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapBal);
  tapBal.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  auto selectTapXOver = [](auto& module) { return &module.acc.tapXOver; };
  tapXOver.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapXOver);
  tapXOver.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapXOver);
  tapXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapXOver);
  tapXOver.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  tapLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  tapLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  tapHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  tapHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbOn = result->params[(int)FFGEchoParam::ReverbOn];
  reverbOn.acc = false;
  reverbOn.name = "Reverb On";
  reverbOn.display = "Reverb";
  reverbOn.slotCount = 1;
  reverbOn.defaultText = "Off";
  reverbOn.id = "{8C47863D-5184-4412-9802-9918DF25DA02}";
  reverbOn.type = FBParamType::Boolean;
  reverbOn.slotFormatDisplay = true;
  auto selectReverbOn = [](auto& module) { return &module.block.reverbOn; };
  reverbOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbOn);
  reverbOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectReverbOn);
  reverbOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbOn);
  reverbOn.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

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
  reverbMix.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& reverbXOver = result->params[(int)FFGEchoParam::ReverbXOver];
  reverbXOver.unit = "%";
  reverbXOver.acc = true;
  reverbXOver.defaultText = "50";
  reverbXOver.name = "Reverb L/R Crossover";
  reverbXOver.matrixName = "Rvrb XOver";
  reverbXOver.display = "XOver";
  reverbXOver.slotCount = 1;
  reverbXOver.id = "{F31A5809-1558-4B9C-A953-EAC108530E3E}";
  reverbXOver.type = FBParamType::Identity;
  auto selectReverbXOver = [](auto& module) { return &module.acc.reverbXOver; };
  reverbXOver.scalarAddr = FFSelectScalarParamAddr(selectModule, selectReverbXOver);
  reverbXOver.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectReverbXOver);
  reverbXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectReverbXOver);
  reverbXOver.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbAPF.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbDamp.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbSize.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackOn = result->params[(int)FFGEchoParam::FeedbackOn];
  feedbackOn.acc = false;
  feedbackOn.name = "Feedback On";
  feedbackOn.display = "Feedback";
  feedbackOn.slotCount = 1;
  feedbackOn.defaultText = "Off";
  feedbackOn.id = "{E28CD2D5-59B4-4C77-830F-BC961239299C}";
  feedbackOn.type = FBParamType::Boolean;
  feedbackOn.slotFormatDisplay = true;
  auto selectFeedbackOn = [](auto& module) { return &module.block.feedbackOn; };
  feedbackOn.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackOn);
  feedbackOn.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackOn);
  feedbackOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackOn);
  feedbackOn.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target }, [](auto const& vs) { return vs[0] != 0; });

  auto& feedbackMix = result->params[(int)FFGEchoParam::FeedbackMix];
  feedbackMix.acc = true;
  feedbackMix.unit = "%";
  feedbackMix.defaultText = "50";
  feedbackMix.name = "Feedback Mix";
  feedbackMix.matrixName = "Fdbk Mix";
  feedbackMix.display = "Mix";
  feedbackMix.slotCount = 1;
  feedbackMix.id = "{47D9FE5B-D161-4198-9AC2-44089966575E}";
  feedbackMix.type = FBParamType::Identity;
  auto selectFeedbackMix = [](auto& module) { return &module.acc.feedbackMix; };
  feedbackMix.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackMix);
  feedbackMix.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackMix);
  feedbackMix.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackMix);
  feedbackMix.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackXOver = result->params[(int)FFGEchoParam::FeedbackXOver];
  feedbackXOver.unit = "%";
  feedbackXOver.acc = true;
  feedbackXOver.defaultText = "0";
  feedbackXOver.name = "Feedback L/R Crossover";
  feedbackXOver.matrixName = "Fdbk XOver";
  feedbackXOver.display = "XOver";
  feedbackXOver.slotCount = 1;
  feedbackXOver.id = "{C4D67CA6-41FE-4B54-8C2B-9C87F0DA6F0C}";
  feedbackXOver.type = FBParamType::Identity;
  auto selectFeedbackXOver = [](auto& module) { return &module.acc.feedbackXOver; };
  feedbackXOver.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackXOver);
  feedbackXOver.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackXOver);
  feedbackXOver.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackXOver);
  feedbackXOver.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackAmount = result->params[(int)FFGEchoParam::FeedbackAmount];
  feedbackAmount.unit = "%";
  feedbackAmount.acc = true;
  feedbackAmount.defaultText = "50";
  feedbackAmount.name = "Feedback Amount";
  feedbackAmount.matrixName = "Fdbk Amt";
  feedbackAmount.display = "Amt";
  feedbackAmount.slotCount = 1;
  feedbackAmount.id = "{56017DC9-4B7D-44A5-AB7C-B6158BA584C3}";
  feedbackAmount.type = FBParamType::Identity;
  auto selectFeedbackAmount = [](auto& module) { return &module.acc.feedbackAmount; };
  feedbackAmount.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackAmount);
  feedbackAmount.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackAmount);
  feedbackAmount.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackAmount);
  feedbackAmount.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackDelayTime = result->params[(int)FFGEchoParam::FeedbackDelayTime];
  feedbackDelayTime.acc = true;
  feedbackDelayTime.defaultText = "1";
  feedbackDelayTime.display = "Dly";
  feedbackDelayTime.name = "Feedback Delay Time";
  feedbackDelayTime.matrixName = "Fdbk Delay Time";
  feedbackDelayTime.slotCount = 1;
  feedbackDelayTime.unit = "Sec";
  feedbackDelayTime.id = "{CBD0A273-5E74-460E-A327-5EE9EE1C6F49}";
  feedbackDelayTime.type = FBParamType::Linear;
  feedbackDelayTime.Linear().min = 0.0f;
  feedbackDelayTime.Linear().max = 10.0f;
  feedbackDelayTime.Linear().editSkewFactor = 0.5f;
  auto selectFeedbackDelayTime = [](auto& module) { return &module.acc.feedbackDelayTime; };
  feedbackDelayTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackDelayTime);
  feedbackDelayTime.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackDelayTime);
  feedbackDelayTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackDelayTime);
  feedbackDelayTime.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  feedbackDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] == 0; });

  auto& feedbackDelayBars = result->params[(int)FFGEchoParam::FeedbackDelayBars];
  feedbackDelayBars.acc = false;
  feedbackDelayBars.defaultText = "1/4";
  feedbackDelayBars.display = "Dly";
  feedbackDelayBars.name = "Feedback Delay Bars";
  feedbackDelayBars.matrixName = "Fdbk Delay Bars";
  feedbackDelayBars.slotCount = 1;
  feedbackDelayBars.unit = "Bars";
  feedbackDelayBars.id = "{BAC85A14-5F60-4692-9D45-81AB29477F61}";
  feedbackDelayBars.type = FBParamType::Bars;
  feedbackDelayBars.Bars().items = MakeGEchoBarsItems();
  auto selectFeedbackDelayBars = [](auto& module) { return &module.block.feedbackDelayBars; };
  feedbackDelayBars.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackDelayBars);
  feedbackDelayBars.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackDelayBars);
  feedbackDelayBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackDelayBars);
  feedbackDelayBars.dependencies.visible.audio.WhenSimple({ (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  feedbackDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  auto& feedbackLPFreq = result->params[(int)FFGEchoParam::FeedbackLPFreq];
  feedbackLPFreq.acc = true;
  feedbackLPFreq.defaultText = std::to_string((int)FFMaxStateVariableFilterFreq);
  feedbackLPFreq.name = "Feedback LPF Freq";
  feedbackLPFreq.matrixName = "Fdbk LPF Freq";
  feedbackLPFreq.display = "Frq";
  feedbackLPFreq.slotCount = 1;
  feedbackLPFreq.unit = "Hz";
  feedbackLPFreq.id = "{E4D8EB30-BF2A-4935-AE14-053F6CF9C446}";
  feedbackLPFreq.type = FBParamType::Log2;
  feedbackLPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectFeedbackLPFreq = [](auto& module) { return &module.acc.feedbackLPFreq; };
  feedbackLPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackLPFreq);
  feedbackLPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackLPFreq);
  feedbackLPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackLPFreq);
  feedbackLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackLPRes = result->params[(int)FFGEchoParam::FeedbackLPRes];
  feedbackLPRes.acc = true;
  feedbackLPRes.defaultText = "0";
  feedbackLPRes.name = "Feedback LPF Res";
  feedbackLPRes.matrixName = "Fdbk LPF Res";
  feedbackLPRes.display = "Res";
  feedbackLPRes.slotCount = 1;
  feedbackLPRes.unit = "%";
  feedbackLPRes.id = "{178C853C-F11C-45C6-A8D3-D62FFA202D9E}";
  feedbackLPRes.type = FBParamType::Identity;
  auto selectFeedbackLPRes = [](auto& module) { return &module.acc.feedbackLPRes; };
  feedbackLPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackLPRes);
  feedbackLPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackLPRes);
  feedbackLPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackLPRes);
  feedbackLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackHPFreq = result->params[(int)FFGEchoParam::FeedbackHPFreq];
  feedbackHPFreq.acc = true;
  feedbackHPFreq.defaultText = std::to_string((int)FFMinStateVariableFilterFreq);
  feedbackHPFreq.name = "Feedback HPF Freq";
  feedbackHPFreq.matrixName = "Fdbk HPF Freq";
  feedbackHPFreq.display = "Frq";
  feedbackHPFreq.slotCount = 1;
  feedbackHPFreq.unit = "Hz";
  feedbackHPFreq.id = "{FE81A57D-1D98-4D3D-B563-8A63B099DB99}";
  feedbackHPFreq.type = FBParamType::Log2;
  feedbackHPFreq.Log2().Init(0.0f, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  auto selectFeedbackHPFreq = [](auto& module) { return &module.acc.feedbackHPFreq; };
  feedbackHPFreq.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackHPFreq);
  feedbackHPFreq.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackHPFreq);
  feedbackHPFreq.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackHPFreq);
  feedbackHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  auto& feedbackHPRes = result->params[(int)FFGEchoParam::FeedbackHPRes];
  feedbackHPRes.acc = true;
  feedbackHPRes.defaultText = "0";
  feedbackHPRes.name = "Feedback HPF Res";
  feedbackHPRes.matrixName = "Fdbk HPF Res";
  feedbackHPRes.display = "Res";
  feedbackHPRes.slotCount = 1;
  feedbackHPRes.unit = "%";
  feedbackHPRes.id = "{FD6F1412-EE09-4A50-835E-A8051AC39411}";
  feedbackHPRes.type = FBParamType::Identity;
  auto selectFeedbackHPRes = [](auto& module) { return &module.acc.feedbackHPRes; };
  feedbackHPRes.scalarAddr = FFSelectScalarParamAddr(selectModule, selectFeedbackHPRes);
  feedbackHPRes.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectFeedbackHPRes);
  feedbackHPRes.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectFeedbackHPRes);
  feedbackHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::FeedbackOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  return result;
}