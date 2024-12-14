#include <playground_plug/shared/FFPlugTopo.hpp> // TODO
#include <playground_plug/shared/FFPlugConfig.hpp> // TODO
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

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
  gain.name = "Gain";
  gain.slotCount = 1;
  gain.valueCount = 0;
  gain.id = "{9CDC04BC-D0FF-43E6-A2C2-D6C822CFA3EA}";
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  return result;
}