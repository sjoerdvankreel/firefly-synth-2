#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFPlugMeta.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

static FBStaticTopoGUI
MakeTopoGUI()
{
  FBStaticTopoGUI result = {};
  result.plugWidth = 800;
  result.minUserScale = 0.5f;
  result.maxUserScale = 16.0f;
  result.aspectRatioWidth = 2;
  result.aspectRatioHeight = 1;
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

static FBSpecialParams
SpecialParamsSelector(
  FBStaticTopo const& topo, void* state)
{
  FBSpecialParams result = {};
  result.hostSmoothTime = MakeSpecialParam(
    topo, state, (int)FFModuleType::Master, (int)FFMasterParam::HostSmoothTime);
  return result;
}

static FBStaticTopoState
MakeTopoState()
{
  FBStaticTopoState result = {};
  result.specialSelector = SpecialParamsSelector;
  result.exchangeStateSize = sizeof(FFExchangeState);
  result.exchangeStateAlignment = alignof(FFExchangeState);
  result.allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result.allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result.allocRawExchangeState = []() { return static_cast<void*>(new FFExchangeState); };
  result.freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result.freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result.freeRawExchangeState = [](void* state) { delete static_cast<FFExchangeState*>(state); };
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
  result->modules[(int)FFModuleType::Master] = std::move(*FFMakeMasterTopo());
  result->modules[(int)FFModuleType::GFilter] = std::move(*FFMakeGFilterTopo());
  return result;
}