#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp> // TODO
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp> // TODO
#include <playground_plug/modules/osci/FFOsciTopo.hpp> // TODO
#include <playground_base/base/topo/FBStaticTopo.hpp>

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->modules.resize(FFModuleCount);
  result->version.major = FF_PLUG_VERSION_MAJOR;
  result->version.minor = FF_PLUG_VERSION_MINOR;
  result->version.patch = FF_PLUG_VERSION_PATCH;
  result->allocProcState = []() { return static_cast<void*>(new FFProcState); };
  result->allocScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->freeProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->freeScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };

  result->modules[FFModuleGLFO] = FFMakeGLFOTopo();
  result->modules[FFModuleOsci] = FFMakeOsciTopo();

  auto& shaper = result->modules[FFModuleShaper];
  auto selectShaper = [](auto& state) { return &state.voice.shaper; };
  shaper.voice = true;
  shaper.name = "Shaper";
  shaper.slotCount = FFShaperCount;
  shaper.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  shaper.params.resize(FFShaperParamCount);

  auto& shaperOn = shaper.params[FFShaperBlockOn];
  shaperOn.acc = false;
  shaperOn.name = "On";
  shaperOn.slotCount = 1;
  shaperOn.valueCount = 2;
  shaperOn.id = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";
  auto selectShaperOn = [](auto& module) { return &module.block.on; };
  shaperOn.scalarAddr = FFTopoDetailSelectScalarAddr(selectShaper, selectShaperOn);
  shaperOn.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectShaper, selectShaperOn);

  auto& shaperClip = shaper.params[FFShaperBlockClip];
  shaperClip.acc = false;
  shaperClip.name = "Clip";
  shaperClip.slotCount = 1;
  shaperClip.valueCount = 2;
  shaperClip.id = "{81C7442E-4064-4E90-A742-FDDEA84AE1AC}";
  auto selectShaperClip = [](auto& module) { return &module.block.clip; };
  shaperClip.scalarAddr = FFTopoDetailSelectScalarAddr(selectShaper, selectShaperClip);
  shaperClip.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectShaper, selectShaperClip);

  auto& shaperGain = shaper.params[FFShaperAccGain];
  shaperGain.acc = true;
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.valueCount = 0;
  shaperGain.id = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";
  auto selectShaperGain = [](auto& module) { return &module.acc.gain; };
  shaperGain.scalarAddr = FFTopoDetailSelectScalarAddr(selectShaper, selectShaperGain);
  shaperGain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectShaper, selectShaperGain);

  auto& shaperGLFOToGain = shaper.params[FFShaperAccGLFOToGain];
  shaperGLFOToGain.acc = true;
  shaperGLFOToGain.name = "GLFO To Gain";
  shaperGLFOToGain.slotCount = 1;
  shaperGLFOToGain.valueCount = 0;
  shaperGLFOToGain.id = "{3AE4ACF7-C0F2-4316-A02B-76DE01BB2A75}";
  auto selectShaperGLFOToGain = [](auto& module) { return &module.acc.glfoToGain; };
  shaperGLFOToGain.scalarAddr = FFTopoDetailSelectScalarAddr(selectShaper, selectShaperGLFOToGain);
  shaperGLFOToGain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectShaper, selectShaperGLFOToGain);

  auto& master = result->modules[FFModuleMaster];
  auto selectMaster = [](auto& state) { return &state.global.master; };
  master.voice = false;
  master.name = "Master";
  master.slotCount = 1;
  master.id = "{83AA98D4-9D12-4D61-81A4-4FAA935EDF5D}";
  master.params.resize(FFMasterParamCount);

  auto& masterGain = master.params[FFMasterAccGain];
  masterGain.acc = true;
  masterGain.name = "Gain";
  masterGain.slotCount = 1;
  masterGain.valueCount = 0;
  masterGain.id = "{9CDC04BC-D0FF-43E6-A2C2-D6C822CFA3EA}";
  auto selectMasterGain = [](auto& module) { return &module.acc.gain; };
  masterGain.scalarAddr = FFTopoDetailSelectScalarAddr(selectMaster, selectMasterGain);
  masterGain.globalAccAddr = FFTopoDetailSelectProcAddr(selectMaster, selectMasterGain);

  return result;
}
