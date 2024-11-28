#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>
#include <cassert>

void
FBHostAudioBlock::CopyFrom(FBPipelineAudioBlock const& pipeline, int srcOffset, int count)
{
  assert(srcOffset + count <= Count());
  assert(0 <= count && count <= pipeline.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      _store[ch][s + srcOffset] = pipeline[ch][s];
}