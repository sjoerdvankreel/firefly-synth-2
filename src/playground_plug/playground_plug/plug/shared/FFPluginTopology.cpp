#include <playground_plug/plug/shared/FFPluginTopology.hpp>

std::unique_ptr<FBPluginStaticTopology>
FFCreateStaticTopology()
{
  auto result = std::make_unique<FBPluginStaticTopology>();
  result->modules.resize(FFModuleCount);
  
  auto& oscillator = result->modules[FFModuleOscillator];
  oscillator.name = "Oscillator";
  oscillator.slotCount = FF_OSCILLATOR_COUNT;
  oscillator.uniqueId = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  oscillator.parameters.resize(FFOscillatorParamCount);

  auto& oscillatorGain = oscillator.parameters[FFOscillatorParamGain];
  oscillatorGain.name = "Gain";
  oscillatorGain.slotCount = 1;
  oscillatorGain.stepCount = 0;
  oscillatorGain.uniqueId = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";

  auto& oscillatorPitch = oscillator.parameters[FFOscillatorParamPitch];
  oscillatorPitch.name = "Pitch";
  oscillatorPitch.slotCount = 1;
  oscillatorPitch.stepCount = 0;
  oscillatorPitch.uniqueId = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";

  auto& waveShaper = result->modules[FFModuleWaveShaper];
  waveShaper.name = "WaveShaper";
  waveShaper.slotCount = FF_WAVE_SHAPER_COUNT;
  waveShaper.uniqueId = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  waveShaper.parameters.resize(FFWaveShaperParamCount);

  auto& waveShaperOn = waveShaper.parameters[FFWaveShaperParamOn];
  waveShaperOn.name = "On";
  waveShaperOn.slotCount = 1;
  waveShaperOn.stepCount = 1;
  waveShaperOn.uniqueId = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";

  auto& waveShaperGain = waveShaper.parameters[FFWaveShaperParamGain];
  waveShaperGain.name = "Gain";
  waveShaperGain.slotCount = 1;
  waveShaperGain.stepCount = 0;
  waveShaperGain.uniqueId = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";

  return result;
}
