#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFDeserializationConverter.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_synth/modules/mix/FFVMixTopo.hpp>
#include <firefly_synth/modules/mix/FFGMixTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/midi/FFMIDITopo.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsState.hpp>

#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

static FBSpecialParam
MakeSpecialParam(
  FBStaticTopo const& topo, void* state, 
  int moduleIndex, int paramIndex)
{
  FBSpecialParam result;
  result.paramIndex = paramIndex;
  result.moduleIndex = moduleIndex;
  auto const& param = topo.modules[moduleIndex].params[paramIndex];
  result.state = param.globalBlockProcAddr(0, 0, state);
  return result;
}

static FBSpecialGUIParam
MakeSpecialGUIParam(
  FBStaticTopo const& topo, void* state,
  int moduleIndex, int paramIndex)
{
  FBSpecialGUIParam result;
  result.paramIndex = paramIndex;
  result.moduleIndex = moduleIndex;
  auto const& param = topo.modules[moduleIndex].guiParams[paramIndex];
  result.state = param.scalarAddr(0, 0, state);
  return result;
}

static FBSpecialParams
SpecialParamsSelector(
  FBStaticTopo const& topo, void* state)
{
  FBSpecialParams result = {};
  result.hostSmoothTime = MakeSpecialParam(
    topo, state, (int)FFModuleType::Master, (int)FFMasterParam::HostSmoothTime);
  return result;
}

static FBSpecialGUIParams
SpecialGUIParamsSelector(
  FBStaticTopo const& topo, void* state)
{
  FBSpecialGUIParams result = {};
  result.userScale = MakeSpecialGUIParam(
    topo, state, (int)FFModuleType::GUISettings, (int)FFGUISettingsGUIParam::UserScale);
  return result;
}

std::string
FFFormatBlockSlot(FBStaticTopo const&, int /* moduleSlot */, int itemSlot)
{
  return std::string(1, static_cast<char>('A' + itemSlot));
}

FBStaticTopoMeta
FFPlugMeta(FBPlugFormat format)
{
  FBStaticTopoMeta result;
  result.format = format;
  result.name = FFPlugName;
  result.id = FFPlugUniqueId;
  result.vendor = FFVendorName;
  result.version.major = FF_PLUG_VERSION_MAJOR;
  result.version.minor = FF_PLUG_VERSION_MINOR;
  result.version.patch = FF_PLUG_VERSION_PATCH;
  return result;
}

std::unique_ptr<FFStaticTopo>
FFMakeTopo(FBPlugFormat format)
{
  auto result = std::make_unique<FFStaticTopo>();
  result->meta = FFPlugMeta(format);
  result->maxUndoSize = 15;
  result->patchExtension = "ff2preset";
  result->guiWidth = 1200;
  result->guiAspectRatioWidth = 32;
  result->guiAspectRatioHeight = 17;
  result->guiFactory = [](FBHostGUIContext* hostContext) { 
    return std::make_unique<FFPlugGUI>(hostContext); };
  result->deserializationConverterFactory = [](FBPlugVersion const& oldVersion, FBRuntimeTopo const* topo) { 
    return std::make_unique<FFDeserializationConverter>(oldVersion, topo); };

  result->specialSelector = SpecialParamsSelector;
  result->specialGUISelector = SpecialGUIParamsSelector;
  result->exchangeStateSize = sizeof(FFExchangeState);
  result->exchangeStateAlignment = alignof(FFExchangeState);
  result->allocRawGUIState = []() { return static_cast<void*>(new FFGUIState); };
  result->allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result->allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->allocRawExchangeState = []() { return static_cast<void*>(new FFExchangeState); };
  result->freeRawGUIState = [](void* state) { delete static_cast<FFGUIState*>(state); };
  result->freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result->freeRawExchangeState = [](void* state) { delete static_cast<FFExchangeState*>(state); };
  result->hostExchangeAddr = [](void* state) { return &static_cast<FFExchangeState*>(state)->host; };
  result->voicesExchangeAddr = [](void* state) { return &static_cast<FFExchangeState*>(state)->voices; };

  result->modules.resize((int)FFModuleType::Count);
  result->modules[(int)FFModuleType::MIDI] = std::move(*FFMakeMIDITopo());
  result->modules[(int)FFModuleType::Master] = std::move(*FFMakeMasterTopo());
  result->modules[(int)FFModuleType::VoiceModule] = std::move(*FFMakeVoiceModuleTopo());
  result->modules[(int)FFModuleType::Output] = std::move(*FFMakeOutputTopo());
  result->modules[(int)FFModuleType::GUISettings] = std::move(*FFMakeGUISettingsTopo());
  result->modules[(int)FFModuleType::Osci] = std::move(*FFMakeOsciTopo());
  result->modules[(int)FFModuleType::OsciMod] = std::move(*FFMakeOsciModTopo());
  result->modules[(int)FFModuleType::VEffect] = std::move(*FFMakeEffectTopo(false));
  result->modules[(int)FFModuleType::VEcho] = std::move(*FFMakeEchoTopo(false));
  result->modules[(int)FFModuleType::GEffect] = std::move(*FFMakeEffectTopo(true));
  result->modules[(int)FFModuleType::GEcho] = std::move(*FFMakeEchoTopo(true));
  result->modules[(int)FFModuleType::VLFO] = std::move(*FFMakeLFOTopo(false));
  result->modules[(int)FFModuleType::GLFO] = std::move(*FFMakeLFOTopo(true));
  result->modules[(int)FFModuleType::Env] = std::move(*FFMakeEnvTopo());
  result->modules[(int)FFModuleType::VMix] = std::move(*FFMakeVMixTopo());
  result->modules[(int)FFModuleType::GMix] = std::move(*FFMakeGMixTopo());

  // These need to go last.
  result->gMatrixSources = FFModMatrixMakeSources(true, result.get());
  result->vMatrixSources = FFModMatrixMakeSources(false, result.get());
  result->gMatrixTargets = FFModMatrixMakeTargets(true, result.get());
  result->vMatrixTargets = FFModMatrixMakeTargets(false, result.get());
  result->modules[(int)FFModuleType::GMatrix] = std::move(*FFMakeModMatrixTopo(true, result.get()));
  result->modules[(int)FFModuleType::VMatrix] = std::move(*FFMakeModMatrixTopo(false, result.get()));

  // This better lines up with the audio engine.
  result->moduleProcessOrder.push_back({ (int)FFModuleType::GUISettings, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::GMatrix, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::MIDI, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::Master, 0 });
  for (int s = 0; s < result->modules[(int)FFModuleType::GLFO].slotCount; s++)
    result->moduleProcessOrder.push_back({ (int)FFModuleType::GLFO, s });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::VMatrix, 0 });
  for (int s = 0; s < FFLFOCount; s++)
  {
    result->moduleProcessOrder.push_back({ (int)FFModuleType::Env, s });
    result->moduleProcessOrder.push_back({ (int)FFModuleType::VLFO, s });
  }
  result->moduleProcessOrder.push_back({ (int)FFModuleType::Env, FFAmpEnvSlot });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::VoiceModule, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::OsciMod, 0 });
  for (int s = 0; s < result->modules[(int)FFModuleType::Osci].slotCount; s++)
    result->moduleProcessOrder.push_back({ (int)FFModuleType::Osci, s });
  for (int s = 0; s < result->modules[(int)FFModuleType::VEffect].slotCount; s++)
    result->moduleProcessOrder.push_back({ (int)FFModuleType::VEffect, s });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::VMix, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::VEcho, 0 });
  for (int s = 0; s < result->modules[(int)FFModuleType::GEffect].slotCount; s++)
    result->moduleProcessOrder.push_back({ (int)FFModuleType::GEffect, s });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::GMix, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::GEcho, 0 });
  result->moduleProcessOrder.push_back({ (int)FFModuleType::Output, 0 });

  return result;
}