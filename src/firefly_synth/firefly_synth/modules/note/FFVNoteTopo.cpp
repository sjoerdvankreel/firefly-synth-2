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

  auto& outputRandomUni = result->cvOutputs[(int)FFVNoteCVOutput::RandomUni];
  outputRandomUni.name = "RndUni";
  outputRandomUni.slotCount = FFVNoteOnNoteRandomCount;
  outputRandomUni.id = "{B1FE4CD9-0F3D-4258-A443-804620DE9B56}";
  outputRandomUni.voiceAddr = [](int, int cvOutputSlot, int voice, void* state) { 
    return &static_cast<FFProcState*>(state)->dsp.voice[voice].vNote.outputRandomUni[cvOutputSlot]; };

  auto& outputRandomNorm = result->cvOutputs[(int)FFVNoteCVOutput::RandomNorm];
  outputRandomNorm.name = "RndNrm";
  outputRandomNorm.slotCount = FFVNoteOnNoteRandomCount;
  outputRandomNorm.id = "{444DC817-385C-43CD-9AF7-5CF5628EE91C}";
  outputRandomNorm.voiceAddr = [](int, int cvOutputSlot, int voice, void* state) { 
    return &static_cast<FFProcState*>(state)->dsp.voice[voice].vNote.outputRandomNorm[cvOutputSlot]; };

  auto& outputGroupRandomUni = result->cvOutputs[(int)FFVNoteCVOutput::GroupRandomUni];
  outputGroupRandomUni.name = "GrpRndUni";
  outputGroupRandomUni.slotCount = FFVNoteOnNoteRandomCount;
  outputGroupRandomUni.id = "{A145E02D-0D88-42FB-ABDA-8FB43489451B}";
  outputGroupRandomUni.voiceAddr = [](int, int cvOutputSlot, int voice, void* state) { 
    return &static_cast<FFProcState*>(state)->dsp.voice[voice].vNote.outputGroupRandomUni[cvOutputSlot]; };

  auto& outputGroupRandomNorm = result->cvOutputs[(int)FFVNoteCVOutput::GroupRandomNorm];
  outputGroupRandomNorm.name = "GrpRndNrm";
  outputGroupRandomNorm.slotCount = FFVNoteOnNoteRandomCount;
  outputGroupRandomNorm.id = "{55CE8AC2-7A33-49F4-A92A-8ED69A509DAE}";
  outputGroupRandomNorm.voiceAddr = [](int, int cvOutputSlot, int voice, void* state) { 
    return &static_cast<FFProcState*>(state)->dsp.voice[voice].vNote.outputGroupRandomNorm[cvOutputSlot]; };
  
  return result;
}