#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/note/FFVNoteTopo.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeVNoteTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "VNote";
  result->slotCount = 1;
  result->id = "{CE604F90-99E9-43D0-BF49-A7EE53BB1F4F}";
  result->cvOutputs.resize((int)FFVNoteCVOutput::Count);

  // todo link to matrix default
  auto& outputKeyUntuned = result->cvOutputs[(int)FFVNoteCVOutput::KeyUntuned];
  outputKeyUntuned.name = "Key";
  outputKeyUntuned.slotCount = 1;
  outputKeyUntuned.id = "{684BB946-8B26-4256-AFF7-4F5C7CF19599}";
  outputKeyUntuned.voiceAddr = [](int, int, int voice, void* state) { return &static_cast<FFProcState*>(state)->dsp.voice[voice].vNote.outputKeyUntuned; };

  auto& outputVelo = result->cvOutputs[(int)FFVNoteCVOutput::Velo];
  outputVelo.name = "Velo";
  outputVelo.slotCount = 1;
  outputVelo.id = "{51E0BEED-59B7-4F23-AC3E-C179756FD8D4}";
  outputVelo.voiceAddr = [](int, int, int voice, void* state) { return &static_cast<FFProcState*>(state)->dsp.voice[voice].vNote.outputVelo; };

  return result;
}