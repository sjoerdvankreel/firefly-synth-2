#include <playground_base/dsp/plug/FBPlugAudioBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>
#include <cassert>

void 
FBPlugAudioBlock::CopyFrom(FBPipelineAudioBlock const& pipeline)
{
  assert(pipeline.Count() >= Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] = pipeline[ch][s];
}