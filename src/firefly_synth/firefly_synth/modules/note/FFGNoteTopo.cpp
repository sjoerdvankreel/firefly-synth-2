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
    case FBNoteMatrixEntry::LastVeloRaw: return "Last VeloRaw";
    case FBNoteMatrixEntry::LastVeloSmooth: return "Last VeloSmth";
    case FBNoteMatrixEntry::LowKeyVeloRaw: return "LowKey VeloRaw";
    case FBNoteMatrixEntry::LowKeyVeloSmooth: return "LowKey VeloSmth";
    case FBNoteMatrixEntry::HighKeyVeloRaw: return "HighKey VeloRaw";
    case FBNoteMatrixEntry::HighKeyVeloSmooth: return "HighKey VeloSmth";
    case FBNoteMatrixEntry::LowVeloVeloRaw: return "LowVelo VeloRaw";
    case FBNoteMatrixEntry::LowVeloVeloSmooth: return "LowVelo VeloSmth";
    case FBNoteMatrixEntry::HighVeloVeloRaw: return "HighVelo VeloRaw";
    case FBNoteMatrixEntry::HighVeloVeloSmooth: return "HighVelo VeloSmth";
    case FBNoteMatrixEntry::LastKeyUntunedRaw: return "Last KeyRaw";
    case FBNoteMatrixEntry::LastKeyUntunedSmooth: return "Last KeySmth";
    case FBNoteMatrixEntry::LowKeyKeyUntunedRaw: return "LowKey KeyRaw";
    case FBNoteMatrixEntry::LowKeyKeyUntunedSmooth: return "LowKey KeySmth";
    case FBNoteMatrixEntry::HighKeyKeyUntunedRaw: return "HighKey KeyRaw";
    case FBNoteMatrixEntry::HighKeyKeyUntunedSmooth: return "HighKey KeySmth";
    case FBNoteMatrixEntry::LowVeloKeyUntunedRaw: return "LowVelo KeyRaw";
    case FBNoteMatrixEntry::LowVeloKeyUntunedSmooth: return "LowVelo KeySmth";
    case FBNoteMatrixEntry::HighVeloKeyUntunedRaw: return "HighVelo KeyRaw";
    case FBNoteMatrixEntry::HighVeloKeyUntunedSmooth: return "HighVelo KeySmth";
    default: FB_ASSERT(false); return "";
    }
  };

  return result;
}