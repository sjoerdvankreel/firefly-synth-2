#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/midi/FFMIDITopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeMIDITopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "MIDI";
  result->slotCount = 1;
  result->id = "{969C6643-6CCB-488A-AD82-00C525161D93}";
  result->cvOutputs.resize((int)FFMIDICVOutput::Count);
  auto selectModule = [](auto& state) { return &state.global.midi; };

  auto& outputCP = result->cvOutputs[(int)FFMIDICVOutput::CP];
  outputCP.name = "CP";
  outputCP.slotCount = 1;
  outputCP.id = "{6C2082BC-5792-4B3E-98C2-5FB783664889}";
  outputCP.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.midi.outputCP; };

  auto& outputPB = result->cvOutputs[(int)FFMIDICVOutput::PB];
  outputPB.name = "PB";
  outputPB.slotCount = 1;
  outputPB.id = "{92F4EC07-961D-4A81-91F4-A892A80C3384}";
  outputPB.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.midi.outputPB; };

  auto& outputCC = result->cvOutputs[(int)FFMIDICVOutput::CC];
  outputCC.name = "CC";
  outputCC.slotCount = FBMIDIEvent::CCMessageCount;
  outputCC.id = "{74C125F5-67FC-4674-8AF9-F303856BCFD7}";
  outputCC.globalAddr = [](int, int os, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.midi.outputCC[os]; };

  return result;
}