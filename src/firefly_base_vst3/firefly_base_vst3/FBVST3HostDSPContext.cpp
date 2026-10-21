#include <firefly_base_vst3/FBVST3Plugin.hpp>

void
FBVST3Plugin::PullDSPAudioClipsFromGUI()
{
  FBVST3AudioClipToDSPCommand command;
  while (_audioClipToDSPCommands.try_dequeue(command))
  {
    switch (command.Type)
    {
    case FBVST3AudioClipToDSPCommand::ClearSingle:
      GetDSPAudioClips().erase(command.id);
      break;
    case FBVST3AudioClipToDSPCommand::LoadSingle:
      GetDSPAudioClips()[command.id] = FBAudioClip(command.clip);
      break;
    case FBVST3AudioClipToDSPCommand::LoadAll:
      GetDSPAudioClips() = command.allClips;
      break;
    default:
      FB_ASSERT(false);
      break;
    }
  }
}