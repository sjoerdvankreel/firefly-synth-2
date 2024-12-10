#include <playground_base/dsp/pipeline/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <cassert>

void 
FBFixedBufferProcessor::BufferFromFixed(FBFixedAudioBlock const& fixed)
{
  _buffer.Append(fixed);
}

void 
FBFixedBufferProcessor::ProcessToHost(FBHostAudioBlock& host)
{
  _hitFixedBlockSize |= _buffer.Count() >= FBFixedAudioBlock::Count();
  if (!_hitFixedBlockSize)
  {
    host.SetToZero(0, host.Count());
    return;
  }

  int padded = std::max(0, host.Count() - _buffer.Count());
  assert(!_paddedOnce || padded == 0);
  _paddedOnce |= padded > 0;
  host.SetToZero(0, padded);

  int used = std::min(host.Count(), _buffer.Count());
  host.CopyFrom(_buffer, padded, used);
  _buffer.Drop(used);
}