#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostOutputBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <cassert>

FBFixedBufferProcessor::
FBFixedBufferProcessor(FBVoiceManager* voiceManager):
_voiceManager(voiceManager)
{
  FBFixedFloatAudioArray lag;
  lag.Fill(0.0f);
  _buffer.AppendFixed(lag);
}

void 
FBFixedBufferProcessor::ProcessToHost(FBHostOutputBlock& host)
{
  host.returnedVoices.clear();
  host.returnedVoices.insert(
    host.returnedVoices.begin(), 
    _returnedVoices.begin(), 
    _returnedVoices.end());
  _returnedVoices.clear();
  assert(host.returnedVoices.size() < FBMaxVoices);

  assert(host.audio.Count() <= _buffer.Count());
  host.audio.CopyFrom(_buffer, host.audio.Count());
  _buffer.Drop(host.audio.Count());
}

void
FBFixedBufferProcessor::BufferFromFixed(FBFixedFloatAudioArray const& fixed)
{
  _buffer.AppendFixed(fixed);
  _returnedVoices.insert(
    _returnedVoices.end(),
    _voiceManager->ReturnedVoices().begin(),
    _voiceManager->ReturnedVoices().end());
  _voiceManager->ResetReturnedVoices();
}