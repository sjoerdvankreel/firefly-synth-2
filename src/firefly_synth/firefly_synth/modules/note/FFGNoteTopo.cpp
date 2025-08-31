#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFTopoDetail.hpp>
#include <firefly_synth/modules/note/FFGNoteTopo.hpp>
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

  // todo smooth
  auto& outputLastKeyRaw = result->cvOutputs[(int)FFGNoteCVOutput::LastKeyRaw];
  outputLastKeyRaw.name = "Last Key Raw";
  outputLastKeyRaw.slotCount = 1;
  outputLastKeyRaw.id = "{FACE9023-643D-4DE8-B3E9-A9CFCD4B821B}";
  outputLastKeyRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.last.keyUntuned.raw; };
  auto& outputLastKeySmth = result->cvOutputs[(int)FFGNoteCVOutput::LastKeySmth];
  outputLastKeySmth.name = "Last Key Smth";
  outputLastKeySmth.slotCount = 1;
  outputLastKeySmth.id = "{2A71B114-A440-4B7C-87B8-1EA887B97D79}";
  outputLastKeySmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.last.keyUntuned.smooth; };
  auto& outputLastVeloRaw = result->cvOutputs[(int)FFGNoteCVOutput::LastVeloRaw];
  outputLastVeloRaw.name = "Last Velo Raw";
  outputLastVeloRaw.slotCount = 1;
  outputLastVeloRaw.id = "{19E9B571-1ECA-44F0-B3F5-1CD54AAEF853}";
  outputLastVeloRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.last.velo.raw; };
  auto& outputLastVeloSmth = result->cvOutputs[(int)FFGNoteCVOutput::LastVeloSmth];
  outputLastVeloSmth.name = "Last Velo Smth";
  outputLastVeloSmth.slotCount = 1;
  outputLastVeloSmth.id = "{056393B4-07AD-47FA-B80D-F43E95BF1A8B}";
  outputLastVeloSmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.last.velo.smooth; };

  auto& outputLowKeyKeyRaw = result->cvOutputs[(int)FFGNoteCVOutput::LowKeyKeyRaw];
  outputLowKeyKeyRaw.name = "Lo Key Key Raw";
  outputLowKeyKeyRaw.slotCount = 1;
  outputLowKeyKeyRaw.id = "{5BCA67BE-0ABA-41D4-85B5-656D1E8E7875}";
  outputLowKeyKeyRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowKey.keyUntuned.raw; };
  auto& outputLowKeyKeySmth = result->cvOutputs[(int)FFGNoteCVOutput::LowKeyKeySmth];
  outputLowKeyKeySmth.name = "Lo Key Key Smth";
  outputLowKeyKeySmth.slotCount = 1;
  outputLowKeyKeySmth.id = "{AA69F47D-914B-4522-87CB-2CC6CD00A119}";
  outputLowKeyKeySmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowKey.keyUntuned.smooth; };
  auto& outputLowKeyVeloRaw = result->cvOutputs[(int)FFGNoteCVOutput::LowKeyVeloRaw];
  outputLowKeyVeloRaw.name = "Lo Key Velo Raw";
  outputLowKeyVeloRaw.slotCount = 1;
  outputLowKeyVeloRaw.id = "{B29F0185-F933-4C11-9E0A-8F1A631F052A}";
  outputLowKeyVeloRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowKey.velo.raw; };
  auto& outputLowKeyVeloSmth = result->cvOutputs[(int)FFGNoteCVOutput::LowKeyVeloSmth];
  outputLowKeyVeloSmth.name = "Lo Key Velo Smth";
  outputLowKeyVeloSmth.slotCount = 1;
  outputLowKeyVeloSmth.id = "{1969CB5B-7446-41EA-8F6A-E5B0CB72F096}";
  outputLowKeyVeloSmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowKey.velo.smooth; };

  auto& outputHighKeyKeyRaw = result->cvOutputs[(int)FFGNoteCVOutput::HighKeyKeyRaw];
  outputHighKeyKeyRaw.name = "Hi Key Key Raw";
  outputHighKeyKeyRaw.slotCount = 1;
  outputHighKeyKeyRaw.id = "{23DF0AD4-5451-4AB5-A10F-3BFBA2781450}";
  outputHighKeyKeyRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highKey.keyUntuned.raw; };
  auto& outputHighKeyKeySmth = result->cvOutputs[(int)FFGNoteCVOutput::HighKeyKeySmth];
  outputHighKeyKeySmth.name = "Hi Key Key Smth";
  outputHighKeyKeySmth.slotCount = 1;
  outputHighKeyKeySmth.id = "{8CF8B121-7B06-42BA-8169-CA9259D7609F}";
  outputHighKeyKeySmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highKey.keyUntuned.smooth; };
  auto& outputHighKeyVeloRaw = result->cvOutputs[(int)FFGNoteCVOutput::HighKeyVeloRaw];
  outputHighKeyVeloRaw.name = "Hi Key Velo Raw";
  outputHighKeyVeloRaw.slotCount = 1;
  outputHighKeyVeloRaw.id = "{DBA62AB2-67BC-48DE-9B1E-35246BC654A2}";
  outputHighKeyVeloRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highKey.velo.raw; };
  auto& outputHighKeyVeloSmth = result->cvOutputs[(int)FFGNoteCVOutput::HighKeyVeloSmth];
  outputHighKeyVeloSmth.name = "Hi Key Velo Smth";
  outputHighKeyVeloSmth.slotCount = 1;
  outputHighKeyVeloSmth.id = "{992A982E-E95D-408B-BA6D-2A0A7A49FBAC}";
  outputHighKeyVeloSmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highKey.velo.smooth; };

  auto& outputLowVeloKeyRaw = result->cvOutputs[(int)FFGNoteCVOutput::LowVeloKeyRaw];
  outputLowVeloKeyRaw.name = "Lo Velo Key Raw";
  outputLowVeloKeyRaw.slotCount = 1;
  outputLowVeloKeyRaw.id = "{734D070A-283C-4C00-A3D2-C42B4327818D}";
  outputLowVeloKeyRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowVelo.keyUntuned.raw; };
  auto& outputLowVeloKeySmth = result->cvOutputs[(int)FFGNoteCVOutput::LowVeloKeySmth];
  outputLowVeloKeySmth.name = "Lo Velo Key Smth";
  outputLowVeloKeySmth.slotCount = 1;
  outputLowVeloKeySmth.id = "{598502ED-3C8D-463A-ACDE-D85BA1FE0F49}";
  outputLowVeloKeySmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowVelo.keyUntuned.smooth; };
  auto& outputLowVeloVeloRaw = result->cvOutputs[(int)FFGNoteCVOutput::LowVeloVeloRaw];
  outputLowVeloVeloRaw.name = "Lo Velo Velo Raw";
  outputLowVeloVeloRaw.slotCount = 1;
  outputLowVeloVeloRaw.id = "{140AB209-E5AD-4DB5-BE40-479A4A6D9F68}";
  outputLowVeloVeloRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowVelo.velo.raw; };
  auto& outputLowVeloVeloSmth = result->cvOutputs[(int)FFGNoteCVOutput::LowVeloVeloSmth];
  outputLowVeloVeloSmth.name = "Lo Velo Velo Smth";
  outputLowVeloVeloSmth.slotCount = 1;
  outputLowVeloVeloSmth.id = "{899A327C-CB76-4EC9-9E1B-18E18D052859}";
  outputLowVeloVeloSmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.lowVelo.velo.smooth; };

  auto& outputHighVeloKeyRaw = result->cvOutputs[(int)FFGNoteCVOutput::HighVeloKeyRaw];
  outputHighVeloKeyRaw.name = "Hi Velo Key Raw";
  outputHighVeloKeyRaw.slotCount = 1;
  outputHighVeloKeyRaw.id = "{F6ADB701-30CD-47B1-9012-9EAF369A8352}";
  outputHighVeloKeyRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highVelo.keyUntuned.raw; };
  auto& outputHighVeloKeySmth = result->cvOutputs[(int)FFGNoteCVOutput::HighVeloKeySmth];
  outputHighVeloKeySmth.name = "Hi Velo Key Smth";
  outputHighVeloKeySmth.slotCount = 1;
  outputHighVeloKeySmth.id = "{8077370A-8427-48D3-B716-7D84AE882E91}";
  outputHighVeloKeySmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highVelo.keyUntuned.smooth; };
  auto& outputHighVeloVeloRaw = result->cvOutputs[(int)FFGNoteCVOutput::HighVeloVeloRaw];
  outputHighVeloVeloRaw.name = "Hi Velo Velo Raw";
  outputHighVeloVeloRaw.slotCount = 1;
  outputHighVeloVeloRaw.id = "{789E8D35-59FA-4472-A9E3-87672F25631E}";
  outputHighVeloVeloRaw.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highVelo.velo.raw; };
  auto& outputHighVeloVeloSmth = result->cvOutputs[(int)FFGNoteCVOutput::HighVeloVeloSmth];
  outputHighVeloVeloSmth.name = "Hi Velo Velo Smth";
  outputHighVeloVeloSmth.slotCount = 1;
  outputHighVeloVeloSmth.id = "{407E8266-49FA-4DA8-929B-5201019ABDFC}";
  outputHighVeloVeloSmth.globalAddr = [](int, int, void* state) { return &static_cast<FFProcState*>(state)->dsp.global.gNote.outputNoteMatrix.highVelo.velo.smooth; };

  return result;
}