#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/dsp/host/FBHostDSPContext.hpp>

#include <clap/helpers/plugin.hxx>

void
FBCLAPPlugin::PullDSPAudioClipsFromGUI()
{
  FBCLAPAudioClipCommand command;
  while (_audioClipCommands.try_dequeue(command))
  {
    if (command.clear)
      GetDSPAudioClips().erase(command.id);
    else
      GetDSPAudioClips()[command.id] = FBAudioClip(command.clip);
  }
}