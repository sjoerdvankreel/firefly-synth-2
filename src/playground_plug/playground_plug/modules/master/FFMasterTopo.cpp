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
  result->params.resize(FFMasterParamCount);
  auto selectModule = [](auto& state) { return &state.global.master; };

  auto& gain = result->params[FFMasterAccGain];
  gain.acc = true;
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.valueCount = 0;
  gain.id = "{9CDC04BC-D0FF-43E6-A2C2-D6C822CFA3EA}";
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);
  return result;
}