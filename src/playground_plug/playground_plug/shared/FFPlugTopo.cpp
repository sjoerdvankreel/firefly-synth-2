#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFPlugMeta.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

static FBSpecialParam
MakeSpecialParam(
  FBStaticTopo const& topo, void* state, 
  int moduleIndex, int paramIndex)
{
  FBSpecialParam result;
  result.paramIndex = paramIndex;
  result.moduleIndex = moduleIndex;
  result.state = topo.modules[moduleIndex].params[paramIndex].globalBlockAddr(0, 0, state);
  return result;
}

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->meta = FFPlugMeta();
  result->modules.resize((int)FFModuleType::Count);
  result->modules[(int)FFModuleType::Env] = std::move(*FFMakeEnvTopo());
  result->modules[(int)FFModuleType::GLFO] = std::move(*FFMakeGLFOTopo());
  result->modules[(int)FFModuleType::Osci] = std::move(*FFMakeOsciTopo());
  result->modules[(int)FFModuleType::Master] = std::move(*FFMakeMasterTopo());
  result->modules[(int)FFModuleType::GFilter] = std::move(*FFMakeGFilterTopo());

  result->gui.plugWidth = 800;
  result->gui.minUserScale = 0.5f;
  result->gui.maxUserScale = 16.0f;
  result->gui.aspectRatioWidth = 2;
  result->gui.aspectRatioHeight = 1;
  result->gui.factory = [](FBHostGUIContext* hostContext) { return std::make_unique<FFPlugGUI>(hostContext); };

  result->state.allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result->state.allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->state.allocRawExchangeState = []() { return static_cast<void*>(new FFExchangeState); };
  result->state.freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->state.freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result->state.freeRawExchangeState = [](void* state) { delete static_cast<FFExchangeState*>(state); };

  result->state.exchangeStateSize = sizeof(FFExchangeState);
  result->state.exchangeStateAlignment = alignof(FFExchangeState);
  result->state.voiceStateExchangeAddr = [](void* state) { return &static_cast<FFExchangeState*>(state)->voiceState; };
  result->state.specialSelector = [](FBStaticTopo const& topo, void* state) {
    FBSpecialParams params = {};
    params.hostSmoothTime = MakeSpecialParam(topo, state, (int)FFModuleType::Master, (int)FFMasterParam::HostSmoothTime);
    return params; };
  return result;
}
