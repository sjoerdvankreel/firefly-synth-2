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
  result->id = "{B0D67581-9D4A-43B0-ADB7-31EED7EC14BD}";
  result->cvOutputs.resize((int)FFGNoteCVOutput::Count);

  auto slotFormatter = [](FBStaticTopo const&, int, int os) {
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

  auto& outputMatrixRaw = result->cvOutputs[(int)FFGNoteCVOutput::NoteMatrixRaw];
  outputMatrixRaw.name = "GNote Raw";
  outputMatrixRaw.slotFormatter = slotFormatter;
  outputMatrixRaw.slotCount = (int)FBNoteMatrixEntry::Count;
  outputMatrixRaw.id = "{FACE9023-643D-4DE8-B3E9-A9CFCD4B821B}";
  outputMatrixRaw.globalAddr = [](int, int cs, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrixRaw.entries[cs]; };

  auto& outputMatrixSmth = result->cvOutputs[(int)FFGNoteCVOutput::NoteMatrixSmth];
  outputMatrixSmth.name = "GNote Smth";
  outputMatrixSmth.slotFormatter = slotFormatter;
  outputMatrixSmth.slotCount = (int)FBNoteMatrixEntry::Count;
  outputMatrixSmth.id = "{158B2D9A-99DE-425E-B3A5-9043C6A73A21}";
  outputMatrixSmth.globalAddr = [](int, int cs, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrixSmth.entries[cs]; };

  return result;
}