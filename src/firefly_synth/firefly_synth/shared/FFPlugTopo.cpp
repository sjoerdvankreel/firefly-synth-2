#include <firefly_synth/shared/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/glfo/FFGLFOTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterTopo.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModTopo.hpp>
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

FBStaticTopoMeta
FFPlugMeta()
{
  FBStaticTopoMeta result;
  result.name = FFPlugName;
  result.id = FFPlugUniqueId;
  result.vendor = FFVendorName;
  result.version.major = FF_PLUG_VERSION_MAJOR;
  result.version.minor = FF_PLUG_VERSION_MINOR;
  result.version.patch = FF_PLUG_VERSION_PATCH;
  return result;
}

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->meta = FFPlugMeta();
  result->guiWidth = 900;
  result->guiAspectRatioWidth = 12;
  result->guiAspectRatioHeight = 7;
  result->guiFactory = [](FBHostGUIContext* hostContext) { return std::make_unique<FFPlugGUI>(hostContext); };

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
  result->modules[(int)FFModuleType::Env] = std::move(*FFMakeEnvTopo());
  result->modules[(int)FFModuleType::GLFO] = std::move(*FFMakeGLFOTopo());
  result->modules[(int)FFModuleType::StringOsci] = std::move(*FFMakeStringOsciTopo());
  result->modules[(int)FFModuleType::Osci] = std::move(*FFMakeOsciTopo());
  result->modules[(int)FFModuleType::OsciMod] = std::move(*FFMakeOsciModTopo());
  result->modules[(int)FFModuleType::Effect] = std::move(*FFMakeEffectTopo());
  result->modules[(int)FFModuleType::Master] = std::move(*FFMakeMasterTopo());
  result->modules[(int)FFModuleType::Output] = std::move(*FFMakeOutputTopo());
  result->modules[(int)FFModuleType::GFilter] = std::move(*FFMakeGFilterTopo());
  result->modules[(int)FFModuleType::GUISettings] = std::move(*FFMakeGUISettingsTopo());
  return result;
}