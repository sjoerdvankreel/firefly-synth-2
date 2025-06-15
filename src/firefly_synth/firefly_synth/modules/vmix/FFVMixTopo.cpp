#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/vmix/FFVMixTopo.hpp>
#include <firefly_synth/modules/vmix/FFVMixState.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeVMixTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->tabName = "VMIX";
  result->name = "Voice Mix";
  result->slotCount = 1;
  result->id = "{5C91D5A0-3EC1-4142-935A-3FE83B60C04E}";
  result->params.resize((int)FFVMixParam::Count);
  result->voiceModuleExchangeAddr = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.vMix; });
  auto selectModule = [](auto& state) { return &state.voice.vMix; };

  auto& osciToVFX = result->params[(int)FFVMixParam::OsciToVFX];
  osciToVFX.acc = true;
  osciToVFX.defaultText = "0";
  osciToVFX.name = "Osci To VFX";
  osciToVFX.slotCount = FFVMixOsciToVFXCount;
  osciToVFX.unit = "%";
  osciToVFX.id = "{1BC03120-9E8C-412B-81D4-17CC662BA72B}";
  osciToVFX.type = FBParamType::Identity;
  auto selectOsciToVFX = [](auto& module) { return &module.acc.osciToVFX; };
  osciToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToVFX);
  osciToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToVFX);
  osciToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToVFX);

  auto& stringOsciToVFX = result->params[(int)FFVMixParam::StringOsciToVFX];
  stringOsciToVFX.acc = true;
  stringOsciToVFX.defaultText = "0";
  stringOsciToVFX.name = "String To VFX";
  stringOsciToVFX.slotCount = FFVMixStringOsciToVFXCount;
  stringOsciToVFX.unit = "%";
  stringOsciToVFX.id = "{5B98D130-B1BA-4E0D-877B-F5656A1EFBE8}";
  stringOsciToVFX.type = FBParamType::Identity;
  auto selectStringOsciToVFX = [](auto& module) { return &module.acc.stringOsciToVFX; };
  stringOsciToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringOsciToVFX);
  stringOsciToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringOsciToVFX);
  stringOsciToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringOsciToVFX);

  /*
  auto& vfxToVFX = result->params[(int)FFVMixParam::VFXToVFX];
  vfxToVFX.acc = true;
  vfxToVFX.defaultText = "0";
  vfxToVFX.name = "VFX To VFX";
  vfxToVFX.slotCount = FFVMixVFXToVFXCount;
  vfxToVFX.unit = "%";
  vfxToVFX.id = "{21EF058C-86B5-4E7B-B4A1-5CFE92F20065}";
  vfxToVFX.type = FBParamType::Identity;
  auto selectVFXToVFX = [](auto& module) { return &module.acc.VFXToVFX; };
  vfxToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVFXToVFX);
  vfxToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectVFXToVFX);
  vfxToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVFXToVFX);
  */

  auto& osciToOut = result->params[(int)FFVMixParam::OsciToOut];
  osciToOut.acc = true;
  osciToOut.defaultText = "0";
  osciToOut.name = "Osci To Out";
  osciToOut.slotCount = FFOsciCount;
  osciToOut.unit = "%";
  osciToOut.id = "{FFBE0515-CF42-4E5F-B453-0CEA58A623D8}";
  osciToOut.type = FBParamType::Identity;
  auto selectOsciToOut = [](auto& module) { return &module.acc.osciToOut; };
  osciToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToOut);
  osciToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToOut);
  osciToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToOut);

  /*
  auto& stringOsciToOut = result->params[(int)FFVMixParam::StringOsciToOut];
  stringOsciToOut.acc = true;
  stringOsciToOut.defaultText = "0";
  stringOsciToOut.name = "String To Out";
  stringOsciToOut.slotCount = FFStringOsciCount;
  stringOsciToOut.unit = "%";
  stringOsciToOut.id = "{251D3B54-1D6D-418B-88B0-E2E80B410F48}";
  stringOsciToOut.type = FBParamType::Identity;
  auto selectStringOsciToOut = [](auto& module) { return &module.acc.stringOsciToOut; };
  stringOsciToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectStringOsciToOut);
  stringOsciToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectStringOsciToOut);
  stringOsciToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectStringOsciToOut);
  */

  auto& vfxToOut = result->params[(int)FFVMixParam::VFXToOut];
  vfxToOut.acc = true;
  vfxToOut.defaultText = "0";
  vfxToOut.name = "VFX To Out";
  vfxToOut.slotCount = FFEffectCount;
  vfxToOut.unit = "%";
  vfxToOut.id = "{D159D4DD-BF49-4208-BEAE-D5BE550AB9FA}";
  vfxToOut.type = FBParamType::Identity;
  auto selectVFXToOut = [](auto& module) { return &module.acc.VFXToOut; };
  vfxToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVFXToOut);
  vfxToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectVFXToOut);
  vfxToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVFXToOut);

  return result;
}