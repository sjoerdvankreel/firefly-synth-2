#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::vector<FBBarsItem>
MakeLFOBarsItems()
{
  return FBMakeBarsItems(true, { 1, 128 }, { 4, 1 });
}

std::unique_ptr<FBStaticModule>
FFMakeLFOTopo(bool global)
{
  std::string prefix = global ? "G" : "V";
  auto result = std::make_unique<FBStaticModule>();
  result->voice = !global;
  result->name = global ? "Global LFO" : "Voice LFO";
  result->tabName = global ? "GLFO" : "VLFO";
  result->graphName = global ? "GLFO" : "VLFO";
  result->slotCount = FFLFOCount;
  result->graphCount = FFLFOBlockCount + 1;
  result->id = prefix + "{6E9EC930-5391-41BB-9EDA-C9B79F3BE745}";
  result->params.resize((int)FFLFOParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vLFO; });
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gLFO; });
  auto selectVoiceModule = [](auto& state) { return &state.voice.vLFO; };
  auto selectGlobalModule = [](auto& state) { return &state.global.gLFO; };

  auto& on = result->params[(int)FFLFOParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.defaultText = "Off";
  on.id = prefix + "{827146C8-2396-47B3-B521-4DEB17F3C321}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOn);
  on.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOn);
  on.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOn);
  on.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOn);
  on.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOn);

  auto& sync = result->params[(int)FFLFOParam::Sync];
  sync.acc = false;
  sync.display = "Sync";
  sync.name = "Tempo Sync";
  sync.slotCount = 1;
  sync.defaultText = "Off";
  sync.id = prefix + "{879655C7-41EC-4089-84E0-A5E244E47F56}";
  sync.type = FBParamType::Boolean;
  auto selectSync = [](auto& module) { return &module.block.sync; };
  sync.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSync);
  sync.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSync);
  sync.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSync);
  sync.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSync);
  sync.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSync);

  auto& oneShot = result->params[(int)FFLFOParam::OneShot];
  oneShot.acc = false;
  oneShot.name = "One Shot";
  oneShot.slotCount = 1;
  oneShot.defaultText = "Off";
  oneShot.id = prefix + "{3A6A7CB5-ACE3-4FB9-9A2C-4908EB3D050D}";
  oneShot.type = FBParamType::Boolean;
  auto selectOneShot = [](auto& module) { return &module.block.oneShot; };
  oneShot.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectOneShot);
  oneShot.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectOneShot);
  oneShot.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectOneShot);
  oneShot.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectOneShot);
  oneShot.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectOneShot);

  auto& hostSnap = result->params[(int)FFLFOParam::HostSnap];
  hostSnap.acc = false;
  hostSnap.name = "Host Snap";
  hostSnap.display = "Snap";
  hostSnap.slotCount = 1;
  hostSnap.defaultText = "Off";
  hostSnap.id = prefix + "{512E8B8B-0724-49BD-BE42-E8877490B1BE}";
  hostSnap.type = FBParamType::Boolean;
  auto selectHostSnap = [](auto& module) { return &module.block.hostSnap; };
  hostSnap.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectHostSnap);
  hostSnap.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectHostSnap);
  hostSnap.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectHostSnap);
  hostSnap.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectHostSnap);
  hostSnap.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectHostSnap);
  hostSnap.dependencies.enabled.audio.When({ (int)FFLFOParam::On }, [global](auto const& vs) { return global && vs[0] != 0; });

  auto& phase = result->params[(int)FFLFOParam::Phase];
  phase.acc = false;
  phase.defaultText = "0";
  phase.name = "Phase";
  phase.slotCount = 1;
  phase.unit = "%";
  phase.id = prefix + "{88E80B77-86BF-49B0-9822-AACEFC6EAB03}";
  phase.type = FBParamType::Identity;
  auto selectPhase = [](auto& module) { return &module.block.phase; };
  phase.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectPhase);
  phase.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectPhase);
  phase.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectPhase);
  phase.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectPhase);
  phase.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectPhase); 
  
  auto& steps = result->params[(int)FFLFOParam::Steps];
  steps.acc = false;
  steps.defaultText = "1";
  steps.name = "Steps";
  steps.slotCount = 1;
  steps.id = prefix + "{F356CD96-80FD-4A45-A2BE-76785CC5463F}";
  steps.type = FBParamType::Discrete;
  steps.Discrete().valueCount = 32;
  steps.Discrete().valueOffset = 1;
  auto selectSteps = [](auto& module) { return &module.block.steps; };
  steps.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSteps);
  steps.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSteps);
  steps.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSteps);
  steps.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSteps);
  steps.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSteps);

  auto& seed = result->params[(int)FFLFOParam::Seed];
  seed.acc = false;
  seed.defaultText = "0";
  seed.name = "Seed";
  seed.slotCount = 1;
  seed.id = prefix + "{3283748D-70B7-41FC-986B-1B76D4E35F48}";
  seed.type = FBParamType::Discrete;
  seed.Discrete().valueCount = FFLFOMaxSeed + 1;
  auto selectSeed = [](auto& module) { return &module.block.seed; };
  seed.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSeed);
  seed.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSeed);
  seed.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSeed);
  seed.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSeed);
  seed.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSeed);

  auto& smoothTime = result->params[(int)FFLFOParam::SmoothTime];
  smoothTime.acc = false;
  smoothTime.defaultText = "0";
  smoothTime.display = "Smth";
  smoothTime.name = "Smooth Time";
  smoothTime.slotCount = 1;
  smoothTime.unit = "Sec";
  smoothTime.id = prefix + "{6064C303-9588-433E-84FE-79B61FDD2ABE}";
  smoothTime.type = FBParamType::Linear;
  smoothTime.Linear().min = 0.0f;
  smoothTime.Linear().max = 10.0f;
  smoothTime.Linear().editSkewFactor = 0.5f;
  auto selectSmoothTime = [](auto& module) { return &module.block.smoothTime; };
  smoothTime.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSmoothTime);
  smoothTime.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSmoothTime);
  smoothTime.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSmoothTime);
  smoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSmoothTime);
  smoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSmoothTime);
  smoothTime.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] == 0; });
  smoothTime.dependencies.enabled.audio.When({ (int)FFLFOParam::On, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] == 0; });

  auto& smoothBars = result->params[(int)FFLFOParam::SmoothBars];
  smoothBars.acc = false;
  smoothBars.defaultText = "Off";
  smoothBars.display = "Smth";
  smoothBars.name = "Smooth Bars";
  smoothBars.slotCount = 1;
  smoothBars.unit = "Bars";
  smoothBars.id = "{B1FEDD9E-47D0-4D3D-981C-E7C8D8A65BC7}";
  smoothBars.type = FBParamType::Bars;
  smoothBars.Bars().items = MakeLFOBarsItems();
  auto selectSmoothBars = [](auto& module) { return &module.block.smoothBars; };
  smoothBars.scalarAddr = FFSelectDualScalarParamAddr(global, selectGlobalModule, selectVoiceModule, selectSmoothBars);
  smoothBars.voiceBlockProcAddr = FFSelectProcParamAddr(selectVoiceModule, selectSmoothBars);
  smoothBars.voiceExchangeAddr = FFSelectExchangeParamAddr(selectVoiceModule, selectSmoothBars);
  smoothBars.globalBlockProcAddr = FFSelectProcParamAddr(selectGlobalModule, selectSmoothBars);
  smoothBars.globalExchangeAddr = FFSelectExchangeParamAddr(selectGlobalModule, selectSmoothBars);
  smoothBars.dependencies.visible.audio.When({ (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0; });
  smoothBars.dependencies.enabled.audio.When({ (int)FFLFOParam::Type, (int)FFLFOParam::Sync }, [](auto const& vs) { return vs[0] != 0 && vs[1] != 0; });

  return result;
}