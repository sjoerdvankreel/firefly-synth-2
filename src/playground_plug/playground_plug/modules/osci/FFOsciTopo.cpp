#include <playground_plug/shared/FFPlugTopo.hpp> // TODO
#include <playground_plug/shared/FFPlugConfig.hpp> // TODO
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

FBStaticModule
FFMakeOsciTopo()
{
  FBStaticModule result = {};
  auto selectModule = [](auto& state) { return &state.voice.osci; };
  result.voice = true;
  result.name = "Osc";
  result.slotCount = FFOsciCount;
  result.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  result.params.resize(FFOsciParamCount);

  auto& on = result.params[FFOsciBlockOn];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.valueCount = 2;
  on.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  on.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& type = result.params[FFOsciBlockType];
  type.acc = false;
  type.name = "Type";
  type.slotCount = 1;
  type.valueCount = FFOsciTypeCount;
  type.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  type.list = {
    { "{2400822D-BFA9-4A43-91E8-2849756DE659}", "Sine" },
    { "{ECE0331E-DD96-446E-9CCA-5B89EE949EB4}", "Saw" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  type.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& gain = result.params[FFOsciAccGain];
  gain.acc = true;
  gain.slotCount = FFOsciGainCount;
  gain.valueCount = 0;
  gain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  auto& pitch = result.params[FFOsciAccPitch];
  pitch.acc = true;
  pitch.name = "Pitch";
  pitch.slotCount = 1;
  pitch.valueCount = 0;
  pitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  auto selectPitch = [](auto& module) { return &module.acc.pitch; };
  pitch.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectPitch);
  pitch.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectPitch);

  auto& GLFOToGain = result.params[FFOsciAccGLFOToGain];
  GLFOToGain.acc = true;
  GLFOToGain.name = "GLFO To Gain";
  GLFOToGain.slotCount = 1;
  GLFOToGain.valueCount = 0;
  GLFOToGain.id = "{5F4BE3D9-EA5F-49D9-B6C5-8FCD0C279B93}";
  auto selectGLFOToGain = [](auto& module) { return &module.acc.glfoToGain; };
  GLFOToGain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGLFOToGain);
  GLFOToGain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGLFOToGain);

  return result;
}