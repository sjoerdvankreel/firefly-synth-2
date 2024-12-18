#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

FBStaticModule
FFMakeOsciTopo()
{
  FBStaticModule result = {};
  result.voice = true;
  result.name = "Osc";
  result.slotCount = FFOsciCount;
  result.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  result.params.resize(FFOsciParamCount);
  auto selectModule = [](auto& state) { return &state.voice.osci; };

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
  type.defaultText = "Saw";
  type.valueCount = FFOsciTypeCount;
  type.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  type.list = {
    { "{2400822D-BFA9-4A43-91E8-2849756DE659}", "Sine" },
    { "{ECE0331E-DD96-446E-9CCA-5B89EE949EB4}", "BLEP Saw" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectType);
  type.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectType);

  auto& gain = result.params[FFOsciAccGain];
  gain.acc = true;
  gain.unit = "%";
  gain.name = "Gain";
  gain.valueCount = 0;
  gain.defaultText = "100";
  gain.displayMultiplier = 100.0f;
  gain.slotCount = FFOsciGainCount;
  gain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  auto& pitch = result.params[FFOsciAccPitch];
  pitch.acc = true;
  pitch.slotCount = 1;
  pitch.valueCount = 0;
  pitch.plainMin = 0.0f;
  pitch.plainMax = 127.0f;
  pitch.name = "Pitch";
  pitch.unit = "Semitones";
  pitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  auto selectPitch = [](auto& module) { return &module.acc.pitch; };
  pitch.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectPitch);
  pitch.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectPitch);

  auto& glfoToGain = result.params[FFOsciAccGLFOToGain];
  glfoToGain.acc = true;
  glfoToGain.unit = "%";
  glfoToGain.name = "GLFO To Gain";
  glfoToGain.slotCount = 1;
  glfoToGain.valueCount = 0;
  glfoToGain.displayMultiplier = 100.0f;
  glfoToGain.id = "{5F4BE3D9-EA5F-49D9-B6C5-8FCD0C279B93}";
  auto selectGLFOToGain = [](auto& module) { return &module.acc.glfoToGain; };
  glfoToGain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGLFOToGain);
  glfoToGain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGLFOToGain);
  return result;
}