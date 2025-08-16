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

  return result;
}