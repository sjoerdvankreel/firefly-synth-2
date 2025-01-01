#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFTopoDetail.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGLFOTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GLFO";
  result->slotCount = FFGLFOCount;
  result->id = "{D89A9DCA-6A8F-48E5-A317-071E688D729E}";
  result->params.resize((int)FFGLFOParam::Count);
  auto selectModule = [](auto& state) { return &state.global.gLFO; };

  auto& on = result->params[(int)FFGLFOParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.valueCount = 2;
  on.id = "{A9741F9B-5E07-40D9-8FC1-73F90363EF0C}";
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  on.globalBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& rate = result->params[(int)FFGLFOParam::Rate];
  rate.acc = true;
  rate.defaultText = "1";
  rate.name = "Rate";
  rate.plainMin = 0.1f;
  rate.plainMax = 20.0f;
  rate.slotCount = 1;
  rate.valueCount = 0;
  rate.unit = "Hz";
  rate.id = "{79BFD05E-98FA-48D4-8D07-C009285EACA7}";
  auto selectRate = [](auto& module) { return &module.acc.rate; };
  rate.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectRate);
  rate.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectRate);
  return result;
}