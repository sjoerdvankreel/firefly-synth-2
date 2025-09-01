#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/note/FFGNoteTopo.hpp>
#include <firefly_base/dsp/shared/FBNoteMatrix.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeGNoteTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GNote";
  result->slotCount = 1;
  result->id = "{F3DD66BB48734}";
  result->cvOutputs.resize((int)FFGNoteCVOutput::Count);

  auto& outputMatrix = result->cvOutputs[(int)FFGNoteCVOutput::NoteMatrix];
  outputMatrix.name = "MATRIX";
  outputMatrix.slotCount = (int)FBNoteMatrixEntry::Count;
  outputMatrix.id = "{FACE9023-643D-4DE8-B3E9-A9CFCD4B821B}";
  outputMatrix.globalAddr = [](int, int cs, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.entries[cs]; };

  return result;
}