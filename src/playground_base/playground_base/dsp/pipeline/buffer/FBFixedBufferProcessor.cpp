#include <playground_base/dsp/pipeline/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/host/FBHostOutputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <cassert>

FBFixedBufferProcessor::
FBFixedBufferProcessor(FBVoiceManager* voiceManager):
_voiceManager(voiceManager) {}

void 
FBFixedBufferProcessor::BufferFromFixed(FBFixedFloatAudioBlock const& fixed)
{
  _buffer.AppendFixed(fixed);
  _returnedVoices.insert(
    _returnedVoices.end(), 
    _voiceManager->ReturnedVoices().begin(), 
    _voiceManager->ReturnedVoices().end());
  _voiceManager->ResetReturnedVoices();
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
    
  _hitFixedBlockSize |= _buffer.Count() >= FBFixedBlockSamples;
  if (!_hitFixedBlockSize)
  {
    host.audio.SetToZero(0, host.audio.Count());
    return;
  }

  int maxUsable = _buffer.Count() - FBFixedBlockSamples;
  int padded = std::max(0, host.audio.Count() - maxUsable);
  assert(!_paddedOnce || padded == 0);
  _paddedOnce |= padded > 0;
  host.audio.SetToZero(0, padded);

  int used = std::min(host.audio.Count(), maxUsable);
  host.audio.CopyFrom(_buffer, padded, used);
  _buffer.Drop(used);
}