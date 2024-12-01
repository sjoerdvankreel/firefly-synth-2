#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/fixed/FBFixedAudioBlock.hpp>
#include <playground_base/dsp/buffer/FBBufferAudioBlock.hpp>
#include <playground_base/dsp/buffer/FBFixedBufferProcessor.hpp>

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
    host.Fill(0, host.Count(), 0.0f);
    return;
  }

  int padded = std::max(0, host.Count() - _buffer.Count());
  assert(!_paddedOnce || padded == 0);
  _paddedOnce |= padded > 0;
  host.Fill(0, padded, 0.0f);

  int used = std::min(host.Count(), _buffer.Count());
  host.CopyFrom(_buffer, padded, 0, used);
  _buffer.Drop(used);
}