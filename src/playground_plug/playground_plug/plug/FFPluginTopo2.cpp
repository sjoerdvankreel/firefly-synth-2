#if 0
#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <utility>

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
  osciOn.valueCount = 2;
  osciOn.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";
  osciOn.plugParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->plugParam.osci[moduleSlot][FFOsciPlugParamOn]; };

  auto& osciType = osci.plugParams[FFOsciPlugParamType];
  osciType.name = "Type";
  osciType.slotCount = 1;
  osciType.valueCount = FFOsciTypeCount;
  osciType.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  osciType.plugParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->plugParam.osci[moduleSlot][FFOsciPlugParamType]; };

  auto& osciGain = osci.autoParams[FFOsciAutoParamGain];
  osciGain.name = "Gain";
  osciGain.slotCount = 1;
  osciGain.valueCount = 0;
  osciGain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";

  // TODO template this stuff, no repeating
  osciGain.scalarAutoParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->scalarAutoParam.osci[moduleSlot][FFOsciAutoParamGain]; };
  osciGain.denseAutoParamAddr = [](int moduleSlot, int paramSlot, FFProcessorMemory* memory) {
    return &memory->denseAutoParam.osci[moduleSlot][FFOsciAutoParamGain]; };
  osciGain.eventPosAutoParamAddr = [](int moduleSlot, int paramSlot, FFProcessorMemory* memory) {
    return &memory->eventPosAutoParam.osci[moduleSlot][FFOsciAutoParamGain]; };

  auto& osciPitch = osci.autoParams[FFOsciAutoParamPitch];
  osciPitch.name = "Pitch";
  osciPitch.slotCount = 1;
  osciPitch.valueCount = 0;
  osciPitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  osciPitch.scalarAutoParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->scalarAutoParam.osci[moduleSlot][FFOsciAutoParamPitch]; };
  osciPitch.denseAutoParamAddr = [](int moduleSlot, int paramSlot, FFProcessorMemory* memory) {
    return &memory->denseAutoParam.osci[moduleSlot][FFOsciAutoParamPitch]; };
  osciPitch.eventPosAutoParamAddr = [](int moduleSlot, int paramSlot, FFProcessorMemory* memory) {
    return &memory->eventPosAutoParam.osci[moduleSlot][FFOsciAutoParamPitch]; };

  auto& shaper = result.modules[FFModuleShaper];
  shaper.name = "Shaper";
  shaper.slotCount = FF_SHAPER_COUNT;
  shaper.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  shaper.plugParams.resize(FFShaperPlugParamCount);
  shaper.autoParams.resize(FFShaperAutoParamCount);

  auto& shaperOn = shaper.plugParams[FFShaperPlugParamOn];
  shaperOn.name = "On";
  shaperOn.slotCount = 1;
  shaperOn.valueCount = 2;
  shaperOn.id = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";
  shaperOn.plugParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->plugParam.shaper[moduleSlot][FFShaperPlugParamOn]; };

  auto& shaperClip = shaper.plugParams[FFShaperPlugParamClip];
  shaperClip.name = "Clip";
  shaperClip.slotCount = 1;
  shaperClip.valueCount = 2;
  shaperClip.id = "{81C7442E-4064-4E90-A742-FDDEA84AE1AC}";
  shaperClip.plugParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->plugParam.shaper[moduleSlot][FFShaperPlugParamClip]; };

  auto& shaperGain = shaper.autoParams[FFShaperAutoParamGain];
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.valueCount = 2;
  shaperGain.id = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";
  shaperGain.scalarAutoParamAddr = [](int moduleSlot, int paramSlot, FFPluginMemory* memory) {
    return &memory->scalarAutoParam.shaper[moduleSlot][FFShaperAutoParamGain]; };
  shaperGain.denseAutoParamAddr = [](int moduleSlot, int paramSlot, FFProcessorMemory* memory) {
    return &memory->denseAutoParam.shaper[moduleSlot][FFShaperAutoParamGain]; };
  shaperGain.eventPosAutoParamAddr = [](int moduleSlot, int paramSlot, FFProcessorMemory* memory) {
    return &memory->eventPosAutoParam.shaper[moduleSlot][FFShaperAutoParamGain]; };

  return FFStaticTopo(result);
}
#endif
