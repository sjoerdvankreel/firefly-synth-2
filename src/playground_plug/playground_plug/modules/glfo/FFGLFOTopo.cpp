#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

FBStaticModule
FFMakeGLFOTopo()
{
  FBStaticModule result = {};
  result.voice = false;
  result.name = "GLFO";
  result.slotCount = FFGLFOCount;
  result.id = "{D89A9DCA-6A8F-48E5-A317-071E688D729E}";
  result.params.resize(FFGLFOParamCount);
  auto selectModule = [](auto& state) { return &state.global.glfo; };

  auto& on = result.params[FFGLFOBlockOn];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.valueCount = 2;
  on.id = "{A9741F9B-5E07-40D9-8FC1-73F90363EF0C}";
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  on.globalBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& rate = result.params[FFGLFOAccRate];
  rate.acc = true;
  rate.name = "Rate";
  rate.slotCount = 1;
  rate.valueCount = 0;
  rate.id = "{79BFD05E-98FA-48D4-8D07-C009285EACA7}";
  auto selectRate = [](auto& module) { return &module.acc.rate; };
  rate.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectRate);
  rate.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectRate);
  return result;
}