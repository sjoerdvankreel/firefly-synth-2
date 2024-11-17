#include <playground_plug/plug/shared/FFPluginTopo.hpp>

FFStaticTopo
FFMakeTopo()
{
  FFStaticTopo result;
  result.modules.resize(FFModuleCount);
  
  auto& osci = result.modules[FFModuleOsci];
  osci.name = "Osc";
  osci.slotCount = FF_OSCI_COUNT;
  osci.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  osci.plugParams.resize(FFOsciPlugParamCount);
  osci.autoParams.resize(FFOsciAutoParamCount);

  auto& osciOn = osci.plugParams[FFOsciPlugParamOn];
  osciOn.name = "On";
  osciOn.slotCount = 1;
  osciOn.stepCount = 1;
  osciOn.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";

  auto& osciGain = osci.autoParams[FFOsciAutoParamGain];
  osciGain.name = "Gain";
  osciGain.slotCount = 1;
  osciGain.stepCount = 0;
  osciGain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";

  auto& osciPitch = osci.autoParams[FFOsciAutoParamPitch];
  osciPitch.name = "Pitch";
  osciPitch.slotCount = 1;
  osciPitch.stepCount = 0;
  osciPitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";

  auto& shaper = result.modules[FFModuleShaper];
  shaper.name = "Shaper";
  shaper.slotCount = FF_SHAPER_COUNT;
  shaper.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  shaper.plugParams.resize(FFShaperPlugParamCount);
  shaper.autoParams.resize(FFShaperAutoParamCount);

  auto& shaperOn = shaper.plugParams[FFShaperPlugParamOn];
  shaperOn.name = "On";
  shaperOn.slotCount = 1;
  shaperOn.stepCount = 1;
  shaperOn.id = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";

  auto& shaperGain = shaper.autoParams[FFShaperAutoParamGain];
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.stepCount = 0;
  shaperGain.id = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";

  return result;
}
