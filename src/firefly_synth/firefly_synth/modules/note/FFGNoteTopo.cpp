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
  outputMatrix.slotFormatter = [](FBStaticTopo const&, int, int os) {
    switch ((FBNoteMatrixEntry)os)
    {
    case FBNoteMatrixEntry::LastVeloRaw: return "Last Velo Raw";
    case FBNoteMatrixEntry::LastVeloSmooth: return "Last Velo Smth";
    case FBNoteMatrixEntry::LowKeyVeloRaw: return "Low Key Velo Raw";
    case FBNoteMatrixEntry::LowKeyVeloSmooth: return "Low Key Velo Smth";
    case FBNoteMatrixEntry::HighKeyVeloRaw: return "High Key Velo Raw";
    case FBNoteMatrixEntry::HighKeyVeloSmooth: return "High Key Velo Smth";
    case FBNoteMatrixEntry::LowVeloVeloRaw: return "Low Velo VeloRaw";
    case FBNoteMatrixEntry::LowVeloVeloSmooth: return "Low Velo Velo Smth";
    case FBNoteMatrixEntry::HighVeloVeloRaw: return "High Velo Velo Raw";
    case FBNoteMatrixEntry::HighVeloVeloSmooth: return "High Velo Velo Smth";
    case FBNoteMatrixEntry::LastKeyUntunedRaw: return "Last Key Raw";
    case FBNoteMatrixEntry::LastKeyUntunedSmooth: return "Last Key Smth";
    case FBNoteMatrixEntry::LowKeyKeyUntunedRaw: return "Low Key Key Raw";
    case FBNoteMatrixEntry::LowKeyKeyUntunedSmooth: return "Low Key Key Smth";
    case FBNoteMatrixEntry::HighKeyKeyUntunedRaw: return "High Key Key Raw";
    case FBNoteMatrixEntry::HighKeyKeyUntunedSmooth: return "High Key Key Smth";
    case FBNoteMatrixEntry::LowVeloKeyUntunedRaw: return "Low Velo Key Raw";
    case FBNoteMatrixEntry::LowVeloKeyUntunedSmooth: return "Low Velo Key Smth";
    case FBNoteMatrixEntry::HighVeloKeyUntunedRaw: return "High Velo Key Raw";
    case FBNoteMatrixEntry::HighVeloKeyUntunedSmooth: return "High Velo Key Smth";
    default: FB_ASSERT(false); return "";
    }
  };

  return result;
}