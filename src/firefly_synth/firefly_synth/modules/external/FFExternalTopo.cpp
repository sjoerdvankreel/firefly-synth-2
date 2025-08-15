#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/external/FFExternalTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeExternalTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "External";
  result->slotCount = 1;
  result->id = "{D9C9CA57-0828-4DF1-BEB8-277B4E8968F4}";
  result->params.resize((int)FFExternalParam::Count);
  auto selectModule = [](auto& state) { return &state.global.external; };

  auto& hostSmoothTime = result->params[(int)FFExternalParam::HostSmoothTime];
  hostSmoothTime.acc = false;
  hostSmoothTime.defaultText = "2";
  hostSmoothTime.name = "MIDI/Automation Smoothing";
  hostSmoothTime.display = "Smooth";
  hostSmoothTime.slotCount = 1;
  hostSmoothTime.unit = "Ms";
  hostSmoothTime.id = "{47B38412-40B9-474E-9305-062E7FF7C800}";
  hostSmoothTime.type = FBParamType::Linear;
  hostSmoothTime.Linear().min = 0.0f;
  hostSmoothTime.Linear().max = 0.2f;
  hostSmoothTime.Linear().editSkewFactor = 0.5f;
  hostSmoothTime.Linear().displayMultiplier = 1000.0f;
  auto selectHostSmoothTime = [](auto& module) { return &module.block.hostSmoothTime; };
  hostSmoothTime.scalarAddr = FFSelectScalarParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalBlockProcAddr = FFSelectProcParamAddr(selectModule, selectHostSmoothTime);
  hostSmoothTime.globalExchangeAddr = FFSelectExchangeParamAddr(selectModule, selectHostSmoothTime);;

  return result;
}