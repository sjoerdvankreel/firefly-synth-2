#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphProcState.hpp>

static FBNoteEvent
MakeNoteC4On()
{
  FBNoteEvent result;
  result.pos = 0;
  result.on = true;
  result.velo = 1.0f;
  result.note.id = 0;
  result.note.key = 60;
  result.note.channel = 0;
  return result;
}

FBGraphProcState::
FBGraphProcState(FBPlugGUI const* plugGUI):
container(*plugGUI->Topo()),
voiceManager(&container),
audio(), notes(), input() 
{
  input.note = &notes;
  input.audio = &audio;
  input.voiceManager = &voiceManager;
  for (int i = 0; i < plugGUI->Topo()->params.size(); i++)
    container.InitProcessing(i, plugGUI->HostContext()->GetParamNormalized(i));
  voiceManager.Lease(MakeNoteC4On());
}