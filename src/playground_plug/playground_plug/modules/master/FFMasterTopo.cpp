#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFTopoDetail.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeMasterTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "Master";
  result->slotCount = 1;
  result->id = "{83AA98D4-9D12-4D61-81A4-4FAA935EDF5D}";
  result->params.resize((int)FFMasterParam::Count);
  auto selectModule = [](auto& state) { return &state.global.master; };

  auto& gain = result->params[(int)FFMasterParam::Gain];
  gain.acc = true;
  gain.defaultText = "100";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.unit = "%";
  gain.id = "{9CDC04BC-D0FF-43E6-A2C2-D6C822CFA3EA}";
  gain.type = FBParamType::Linear;
  gain.linear.displayMultiplier = 100.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  auto& smoothing = result->params[(int)FFMasterParam::Smoothing];
  smoothing.acc = false;
  smoothing.defaultText = "2";
  smoothing.name = "Smooth";
  smoothing.slotCount = 1;
  smoothing.unit = "Ms";
  smoothing.id = "{961D1B53-9509-47EA-B646-C948C5FACA82}";
  smoothing.type = FBParamType::Linear;
  smoothing.linear.min = 0.0f;
  smoothing.linear.max = 0.05f;
  smoothing.linear.displayMultiplier = 1000.0f;
  auto selectSmoothing = [](auto& module) { return &module.block.smoothing; };
  smoothing.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectSmoothing);
  smoothing.globalBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectSmoothing);
  return result;
}