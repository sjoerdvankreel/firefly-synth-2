#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

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

  return result;
}