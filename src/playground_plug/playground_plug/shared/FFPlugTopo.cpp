#include <playground_plug/shared/FFPlugGUI.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFGUIState.hpp>
#include <playground_plug/shared/FFPlugMeta.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsTopo.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsState.hpp>

#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

static FBStaticTopoGUI
MakeTopoGUI()
{
  FBStaticTopoGUI result = {};
  result.plugWidth = 800;
  result.aspectRatioWidth = 10;
  result.aspectRatioHeight = 7;
  result.factory = [](FBHostGUIContext* hostContext) {
    return std::make_unique<FFPlugGUI>(hostContext); };
  return result;
}

static FBSpecialParam
MakeSpecialParam(
  FBStaticTopo const& topo, void* state, 
  int moduleIndex, int paramIndex)
{
  FBSpecialParam result;
  result.paramIndex = paramIndex;
  result.moduleIndex = moduleIndex;
  auto const& param = topo.modules[moduleIndex].params[paramIndex];
  result.state = param.addrSelectors.globalBlockProc(0, 0, state);
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
  result.state = param.addrSelector(0, 0, state);
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

static FBStaticTopoState
MakeTopoState()
{
  FBStaticTopoState result = {};
  result.specialSelector = SpecialParamsSelector;
  result.specialGUISelector = SpecialGUIParamsSelector;
  result.exchangeStateSize = sizeof(FFExchangeState);
  result.exchangeStateAlignment = alignof(FFExchangeState);
  result.allocRawGUIState = []() { return static_cast<void*>(new FFGUIState); };
  result.allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result.allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result.allocRawExchangeState = []() { return static_cast<void*>(new FFExchangeState); };
  result.freeRawGUIState = [](void* state) { delete static_cast<FFGUIState*>(state); };
  result.freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result.freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result.freeRawExchangeState = [](void* state) { delete static_cast<FFExchangeState*>(state); };
  result.hostExchangeAddr = [](void* state) { return &static_cast<FFExchangeState*>(state)->host; };
  result.voicesExchangeAddr = [](void* state) { return &static_cast<FFExchangeState*>(state)->voices; };
  return result;
}

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->meta = FFPlugMeta();
  result->gui = MakeTopoGUI();
  result->state = MakeTopoState();
  result->modules.resize((int)FFModuleType::Count);
  result->modules[(int)FFModuleType::Env] = std::move(*FFMakeEnvTopo());
  result->modules[(int)FFModuleType::GLFO] = std::move(*FFMakeGLFOTopo());
  result->modules[(int)FFModuleType::Osci] = std::move(*FFMakeOsciTopo());
  result->modules[(int)FFModuleType::OsciAM] = std::move(*FFMakeOsciAMTopo());
  result->modules[(int)FFModuleType::OsciFM] = std::move(*FFMakeOsciFMTopo());
  result->modules[(int)FFModuleType::Master] = std::move(*FFMakeMasterTopo());
  result->modules[(int)FFModuleType::GFilter] = std::move(*FFMakeGFilterTopo());
  result->modules[(int)FFModuleType::GUISettings] = std::move(*FFMakeGUISettingsTopo());
  return result;
}