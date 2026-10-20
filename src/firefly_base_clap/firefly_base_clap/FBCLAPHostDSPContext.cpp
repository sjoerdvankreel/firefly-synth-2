#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/dsp/host/FBHostDSPContext.hpp>

#include <clap/helpers/plugin.hxx>

void
FBCLAPPlugin::PullDSPAudioClipsFromGUI()
{
  FBCLAPAudioClipToDSPCommand command;
  while (_audioClipToDSPCommands.try_dequeue(command))
  {
    switch (command.Type)
    {
    case FBCLAPAudioClipToDSPCommand::ClearSingle:
      GetDSPAudioClips().erase(command.id);
      break;
    case FBCLAPAudioClipToDSPCommand::LoadSingle:
      GetDSPAudioClips()[command.id] = FBAudioClip(command.clip);
      break;
    case FBCLAPAudioClipToDSPCommand::LoadAll:
      GetDSPAudioClips() = command.allClips;
      break;
    default:
      FB_ASSERT(false);
      break;
    }
  }
  _writingAudioClipsToDSPIsDirty.store(false);
}