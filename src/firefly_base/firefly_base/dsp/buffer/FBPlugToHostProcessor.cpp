#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>
#include <firefly_base/dsp/buffer/FBPlugToHostProcessor.hpp>

#include <cassert>

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
  hostBlock.returnedVoices.clear();
  hostBlock.returnedVoices.insert(
    hostBlock.returnedVoices.begin(),
    _returnedVoices.begin(), 
    _returnedVoices.end());
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