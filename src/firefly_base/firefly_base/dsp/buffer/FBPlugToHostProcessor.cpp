#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>
#include <firefly_base/dsp/buffer/FBPlugToHostProcessor.hpp>

FBPlugToHostProcessor::
FBPlugToHostProcessor(FBVoiceManager* voiceManager):
_voiceManager(voiceManager)
{
  FBSArray2<float, FBFixedBlockSamples, 2> lag;
  lag.Fill(0.0f);
  _buffer.AppendPlugAudio(lag);
}

void 
FBPlugToHostProcessor::ProcessToHost(FBHostOutputBlock& hostBlock)
{
  // Check what was returned by the voice manager this round.
  // This is a CLAP-only feature that allows a host to determine
  // when it can stop processing it's polyphonic modulators for
  // any particular note event (which is it's notion of a synth voice).
  // In case of global unison, we kick off multiple voices per note.
  // So, we need to check the global uni group id and only mark a
  // note as finished to the CLAP host when all voices spawned by
  // that note have been completed.
  hostBlock.returnedVoices.clear();
  for (int i = 0; i < _returnedVoices.size(); i++)
  {
    bool returnedVoiceWasLastActiveInGroup = true;
    for (int j = 0; j < FBMaxVoices; j++)
      if (_voiceManager->Voices()[j].state == FBVoiceState::Active)
        if (_voiceManager->Voices()[j].groupId == _returnedVoices[i].groupId)
        {
          returnedVoiceWasLastActiveInGroup = false;
          break;
        }
    if (!returnedVoiceWasLastActiveInGroup)
      continue;
    hostBlock.returnedVoices.push_back(_returnedVoices[i].event.note);
  }
  _returnedVoices.clear();
  FB_ASSERT(hostBlock.returnedVoices.size() < FBMaxVoices);

  FB_ASSERT(hostBlock.audio.Count() <= _buffer.Count());
  hostBlock.audio.CopyFrom(_buffer, hostBlock.audio.Count());
  _buffer.Drop(hostBlock.audio.Count());
}

void
FBPlugToHostProcessor::BufferFromPlug(FBSArray2<float, FBFixedBlockSamples, 2> const& plugAudio)
{
  _buffer.AppendPlugAudio(plugAudio);
  _returnedVoices.insert(
    _returnedVoices.end(),
    _voiceManager->ReturnedVoices().begin(),
    _voiceManager->ReturnedVoices().end());
  _voiceManager->ResetReturnedVoices();
}