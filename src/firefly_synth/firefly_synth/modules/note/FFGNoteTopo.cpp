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
  outputMatrix.name = "GNote";
  outputMatrix.slotCount = (int)FBNoteMatrixEntry::Count;
  outputMatrix.id = "{FACE9023-643D-4DE8-B3E9-A9CFCD4B821B}";
  outputMatrix.globalAddr = [](int, int cs, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.entries[cs]; };
  outputMatrix.slotFormatter = [](FBStaticTopo const&, int, int os) {
    switch ((FBNoteMatrixEntry)os)
    {
    case FBNoteMatrixEntry::LastVelo: return "Last Velo";
    case FBNoteMatrixEntry::LowKeyVelo: return "LowKey Velo";
    case FBNoteMatrixEntry::HighKeyVelo: return "HighKey Velo";
    case FBNoteMatrixEntry::LowVeloVelo: return "LowVelo Velo";
    case FBNoteMatrixEntry::HighVeloVelo: return "HighVelo Velo";
    case FBNoteMatrixEntry::LastKeyUntuned: return "Last Key";
    case FBNoteMatrixEntry::LowKeyKeyUntuned: return "LowKey Key";
    case FBNoteMatrixEntry::HighKeyKeyUntuned: return "HighKey Key";
    case FBNoteMatrixEntry::LowVeloKeyUntuned: return "LowVelo Key";
    case FBNoteMatrixEntry::HighVeloKeyUntuned: return "HighVelo Key";
    default: FB_ASSERT(false); return "";
    }
  };

  return result;
}