#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

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
  tapLPOn.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectTapBal);
  tapLPOn.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectTapBal);

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

  return result;
}