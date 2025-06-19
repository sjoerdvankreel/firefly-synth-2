#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/mix/FFVMixTopo.hpp>
#include <firefly_synth/modules/mix/FFVMixState.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static std::string
FormatOsciToOutSlot(FBStaticTopo const& topo, int mixSlot)
{
  int osciSlot = mixSlot;
  std::string osciName = topo.modules[(int)FFModuleType::Osci].name + " " + std::to_string(osciSlot + 1);
  return osciName + "\U00002192Out";
}

static std::string
FormatOsciToVFXSlot(FBStaticTopo const& topo, int mixSlot)
{
  int fxSlot = FFVMixOsciToVFXGetFXSlot(mixSlot);
  int osciSlot = FFVMixOsciToVFXGetOsciSlot(mixSlot);
  std::string fxName = "FX " + std::to_string(fxSlot + 1);
  std::string osciName = topo.modules[(int)FFModuleType::Osci].name + " " + std::to_string(osciSlot + 1);
  return osciName + "\U00002192" + fxName;
}

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
  osciToVFX.slotFormatter = FormatOsciToVFXSlot;
  osciToVFX.slotFormatterOverrides = true;
  osciToVFX.type = FBParamType::Identity;
  auto selectOsciToVFX = [](auto& module) { return &module.acc.osciToVFX; };
  osciToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToVFX);
  osciToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToVFX);
  osciToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToVFX);

  auto& vfxToVFX = result->params[(int)FFVMixParam::VFXToVFX];
  vfxToVFX.acc = true;
  vfxToVFX.defaultText = "0";
  vfxToVFX.name = "VFX To VFX";
  vfxToVFX.slotCount = FFMixFXToFXCount;
  vfxToVFX.unit = "%";
  vfxToVFX.id = "{21EF058C-86B5-4E7B-B4A1-5CFE92F20065}";
  vfxToVFX.slotFormatter = FFMixFormatFXToFXSlot;
  vfxToVFX.slotFormatterOverrides = true;
  vfxToVFX.type = FBParamType::Identity;
  auto selectVFXToVFX = [](auto& module) { return &module.acc.VFXToVFX; };
  vfxToVFX.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVFXToVFX);
  vfxToVFX.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectVFXToVFX);
  vfxToVFX.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVFXToVFX);  

  auto& osciToOut = result->params[(int)FFVMixParam::OsciToOut];
  osciToOut.acc = true;
  osciToOut.defaultTextSelector = [](int /*ms*/, int ps) { return ps == 0 ? "100" : "0"; };
  osciToOut.name = "Osci To Out";
  osciToOut.slotCount = FFOsciCount;
  osciToOut.unit = "%";
  osciToOut.id = "{FFBE0515-CF42-4E5F-B453-0CEA58A623D8}";
  osciToOut.slotFormatter = FormatOsciToOutSlot;
  osciToOut.slotFormatterOverrides = true;
  osciToOut.type = FBParamType::Identity;
  auto selectOsciToOut = [](auto& module) { return &module.acc.osciToOut; };
  osciToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOsciToOut);
  osciToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectOsciToOut);
  osciToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOsciToOut);

  auto& vfxToOut = result->params[(int)FFVMixParam::VFXToOut];
  vfxToOut.acc = true;
  vfxToOut.defaultText = "0";
  vfxToOut.name = "VFX To Out";
  vfxToOut.slotCount = FFEffectCount;
  vfxToOut.unit = "%";
  vfxToOut.id = "{D159D4DD-BF49-4208-BEAE-D5BE550AB9FA}";
  vfxToOut.slotFormatter = FFMixFormatFXToOutSlot;
  vfxToOut.slotFormatterOverrides = true;
  vfxToOut.type = FBParamType::Identity;
  auto selectVFXToOut = [](auto& module) { return &module.acc.VFXToOut; };
  vfxToOut.scalarAddr = FFSelectScalarParamAddr(selectModule, selectVFXToOut);
  vfxToOut.voiceAccProcAddr = FFSelectProcParamAddr(selectModule, selectVFXToOut);
  vfxToOut.voiceExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectVFXToOut);

  return result;
}