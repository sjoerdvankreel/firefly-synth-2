#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

void 
FBFixedAudioBlock::CopyFrom(FBBufferAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      Sample(ch, s, rhs[ch][s]);
}