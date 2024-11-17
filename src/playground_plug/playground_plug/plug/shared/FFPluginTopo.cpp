#include <playground_plug/plug/shared/FFPluginTopo.hpp>

std::unique_ptr<FBStaticTopo>
FFCreateStaticTopology()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->modules.resize(FFModuleCount);
  
  auto& osci = result->modules[FFModuleOsci];
  osci.name = "Osc";
  osci.slotCount = FF_OSCI_COUNT;
  osci.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  osci.params.resize(FFOsciParamCount);

  auto& osciGain = osci.params[FFOsciParamGain];
  osciGain.name = "Gain";
  osciGain.slotCount = 1;
  osciGain.stepCount = 0;
  osciGain.uniqueId = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";

  auto& osciPitch = osci.params[FFOsciParamPitch];
  osciPitch.name = "Pitch";
  osciPitch.slotCount = 1;
  osciPitch.stepCount = 0;
  osciPitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";

  auto& shaper = result->modules[FFModuleShaper];
  shaper.name = "Shaper";
  shaper.slotCount = FF_SHAPER_COUNT;
  shaper.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  shaper.params.resize(FFShaperParamCount);

  auto& shaperOn = shaper.params[FFShaperParamOn];
  shaperOn.name = "On";
  shaperOn.slotCount = 1;
  shaperOn.stepCount = 1;
  shaperOn.id = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";

  auto& shaperGain = shaper.params[FFShaperParamGain];
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.stepCount = 0;
  shaperGain.id = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";

  return result;
}
