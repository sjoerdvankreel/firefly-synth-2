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
    { "{90701058-1399-4D0A-B098-AE5AFFB9123C}", "Off" },
    { "{9A195809-A9A8-454A-ACD1-5376892EE416}", "Voice" },
    { "{38A69D55-AB3E-49F2-9CCD-44917FD24597}", "Out" },
    { "{CDE64B50-C65D-43E9-B52E-325F544E5175}", "GFX1In" },
    { "{1C7AA2CC-2116-45E4-B7C2-AA9678CC15B5}", "GFX1Out" },
    { "{68C1469F-96FF-47D7-8A2E-B24309E10031}", "GFX2In" },
    { "{A043114B-C471-46A8-A526-B93E1897B1A0}", "GFX2Out" },
    { "{883FA130-AB68-42D5-B56A-C61039DAD9A6}", "GFX3In" },
    { "{56CE90EE-F9D0-4CCF-93B8-9BB0501DCBEF}", "GFX3Out" },
    { "{80ECC9AA-AC6B-44DC-B305-43057720A4AC}", "GFX4In" },
    { "{050E8D54-BFD2-4821-B14F-717D161D69DB}", "GFX4Out" } };
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
  tapsMix.name = "Mix";
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
  tapOn.defaultTextSelector = [](int, int, int ps) { return ps == 0 ? "On" : "Off"; };
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
  tapLevel.defaultTextSelector = [](int, int, int ps) { return std::to_string((int)((1.0f - ps / (float)FFGEchoTapCount) * 100)); };
  tapLevel.type = FBParamType::Identity;
  auto selectTapLevel = [](auto& module) { return &module.acc.tapLevel; };
  tapLevel.scalarAddr = FFSelectScalarParamAddr(selectModule, selectTapLevel);
  tapLevel.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapLevel);
  tapLevel.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapLevel);
  tapLevel.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });
    
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
  tapDelayTime.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] == 0; });

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
  tapDelayBars.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  auto selectTapXOver = [](auto& module) { return &module.acc.tapCrossOver; };
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
  tapLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::TapLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  tapLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::TapLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  tapHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::TapHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  tapHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::TapOn, (int)FFGEchoParam::TapHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  reverbMix.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbSpread.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbAPF.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbDamp.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbSize.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

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
  reverbLPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement, (int)FFGEchoParam::ReverbLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  reverbLPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement, (int)FFGEchoParam::ReverbLPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  reverbHPFreq.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement, (int)FFGEchoParam::ReverbHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

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
  reverbHPRes.dependencies.enabled.audio.WhenSimple({ (int)FFGEchoParam::Target, (int)FFGEchoParam::ReverbPlacement, (int)FFGEchoParam::ReverbHPOn }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0 && vs[2] != 0; });

  /*

 enum class FFGEchoParam {
  
   FeedbackOn, FeedbackMix, FeedbackAmount, FeedbackXOver,
   FeedbackDelayTime, FeedbackDelayBars,
   FeedbackLPFreq, FeedbackLPRes, FeedbackHPFreq, FeedbackHPRes,
   Count };
   */


  return result;
}