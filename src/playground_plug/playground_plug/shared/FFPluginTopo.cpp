#include <playground_plug/shared/FFPluginTopo.hpp>
#include <playground_plug/shared/FFPluginConfig.hpp>

template <class Selector>
auto SelectScalarAddr(Selector selector)
{
  return [selector](
    int moduleSlot, int paramSlot,
    FBScalarAddrsBase& addrs) {
      auto store = selector(dynamic_cast<FFScalarMemory&>(addrs));
      return &(*store)[moduleSlot][paramSlot]; };
}

template <class Selector>
auto SelectProcAddr(Selector selector)
{
  return [selector](
    int moduleSlot, int paramSlot,
    FBProcAddrsBase& addrs) {
      auto store = selector(dynamic_cast<FFProcMemory&>(addrs));
      return &(*store)[moduleSlot][paramSlot]; };
}

FBStaticTopo
FFMakeTopo()
{
  FBStaticTopo result;
  result.modules.resize(FFModuleCount);
  
  auto& osci = result.modules[FFModuleOsci];
  osci.name = "Osc";
  osci.slotCount = FF_OSCI_COUNT;
  osci.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  osci.acc.resize(FFOsciAccCount);
  osci.block.resize(FFOsciBlockCount);

  auto& osciOn = osci.block[FFOsciBlockOn];
  osciOn.name = "On";
  osciOn.slotCount = 1;
  osciOn.valueCount = 2;
  osciOn.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";
  osciOn.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.block.osci.on; });

  auto& osciType = osci.block[FFOsciBlockType];
  osciType.name = "Type";
  osciType.slotCount = 1;
  osciType.valueCount = FFOsciTypeCount;
  osciType.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  osciType.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.block.osci.type; });

  auto& osciGain = osci.acc[FFOsciAccGain];
  osciGain.name = "Gain";
  osciGain.slotCount = 1;
  osciGain.valueCount = 0;
  osciGain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  osciGain.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.acc.osci.gain; });
  osciGain.posAddr = SelectProcAddr([](auto& mem) { return &mem.pos.osci.gain; });
  osciGain.cvAddr = SelectProcAddr([](auto& mem) { return &mem.cv.osci.gain; });

  auto& osciPitch = osci.acc[FFOsciAccPitch];
  osciPitch.name = "Pitch";
  osciPitch.slotCount = 1;
  osciPitch.valueCount = 0;
  osciPitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  osciPitch.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.acc.osci.pitch; });
  osciPitch.posAddr = SelectProcAddr([](auto& mem) { return &mem.pos.osci.pitch; });
  osciPitch.cvAddr = SelectProcAddr([](auto& mem) { return &mem.cv.osci.pitch; });

  auto& shaper = result.modules[FFModuleShaper];
  shaper.name = "Shaper";
  shaper.slotCount = FF_SHAPER_COUNT;
  shaper.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  shaper.acc.resize(FFShaperAccCount);
  shaper.block.resize(FFShaperBlockCount);

  auto& shaperOn = shaper.block[FFShaperBlockOn];
  shaperOn.name = "On";
  shaperOn.slotCount = 1;
  shaperOn.valueCount = 2;
  shaperOn.id = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";
  shaperOn.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.block.shaper.on; });

  auto& shaperClip = shaper.block[FFShaperBlockClip];
  shaperClip.name = "Clip";
  shaperClip.slotCount = 1;
  shaperClip.valueCount = 2;
  shaperClip.id = "{81C7442E-4064-4E90-A742-FDDEA84AE1AC}";
  shaperClip.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.block.shaper.clip; });

  auto& shaperGain = shaper.acc[FFShaperAccGain];
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.valueCount = 2;
  shaperGain.id = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";
  shaperGain.scalarAddr = SelectScalarAddr([](auto& mem) { return &mem.acc.shaper.gain; });
  shaperGain.posAddr = SelectProcAddr([](auto& mem) { return &mem.pos.shaper.gain; });
  shaperGain.cvAddr = SelectProcAddr([](auto& mem) { return &mem.cv.shaper.gain; });

  return FBStaticTopo(result);
}
