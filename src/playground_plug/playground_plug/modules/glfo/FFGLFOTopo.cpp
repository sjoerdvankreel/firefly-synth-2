#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGLFOTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GLFO";
  result->slotCount = FFGLFOCount;
  result->graphRenderer = FFGLFORenderGraph;
  result->id = "{D89A9DCA-6A8F-48E5-A317-071E688D729E}";
  result->params.resize((int)FFGLFOParam::Count);
  result->globalModuleExchangeAddr = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gLFO; });
  auto selectModule = [](auto& state) { return &state.global.gLFO; };

  auto& on = result->params[(int)FFGLFOParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{A9741F9B-5E07-40D9-8FC1-73F90363EF0C}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFSelectScalarParamAddr(selectModule, selectOn);
  on.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectOn);
  on.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& rate = result->params[(int)FFGLFOParam::Rate];
  rate.acc = true;
  rate.defaultText = "1";
  rate.name = "Rate";
  rate.slotCount = 1;
  rate.unit = "Hz";
  rate.id = "{79BFD05E-98FA-48D4-8D07-C009285EACA7}";
  rate.type = FBParamType::Linear;
  rate.Linear().min = 0.1f;
  rate.Linear().max = 20.0f;
  auto selectRate = [](auto& module) { return &module.acc.rate; };
  rate.scalarAddr = FFSelectScalarParamAddr(selectModule, selectRate);
  rate.globalAccProcAddr = FFSelectProcParamAddr(selectModule, selectRate);
  rate.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectRate);
  return result;
}