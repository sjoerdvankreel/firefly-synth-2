#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

FBStaticModule
FFMakeMasterTopo()
{
  FBStaticModule result = {};
  result.voice = false;
  result.name = "Master";
  result.slotCount = 1;
  result.id = "{83AA98D4-9D12-4D61-81A4-4FAA935EDF5D}";
  result.params.resize(FFMasterParamCount);
  auto selectModule = [](auto& state) { return &state.global.master; };

  auto& gain = result.params[FFMasterAccGain];
  gain.acc = true;
  gain.unit = "%";
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.valueCount = 0;
  gain.displayMultiplier = 100.0f;
  gain.id = "{9CDC04BC-D0FF-43E6-A2C2-D6C822CFA3EA}";
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  auto& smooth = result.params[FFMasterBlockSmooth];
  smooth.acc = false;
  smooth.unit = "Ms";
  smooth.name = "Smooth";
  smooth.id = "{961D1B53-9509-47EA-B646-C948C5FACA82}";
  smooth.slotCount = 1;
  smooth.valueCount = 0;
  smooth.plainMin = 0.0f;
  smooth.plainMax = 0.05f;
  smooth.defaultNormalized = 0.5f;
  smooth.displayMultiplier = 1000.0f;
  auto selectSmooth = [](auto& module) { return &module.block.smooth; };
  smooth.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectSmooth);
  smooth.globalBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectSmooth);
  return result;
}