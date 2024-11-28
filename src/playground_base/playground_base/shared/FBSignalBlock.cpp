#include <playground_base/shared/FBSignalBlock.hpp>
#include <cassert>

void
FBPipelineAudioBlock::Append(FBPlugAudioBlock const& plug)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < plug.Count(); s++)
      _store[ch].push_back(plug[ch][s]);
}

void
FBPipelineAudioBlock::Append(FBHostAudioBlock const& host)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < host.Count(); s++)
      _store[ch].push_back(host[ch][s]);
}

void
FBHostAudioBlock::CopyFrom(FBPipelineAudioBlock const& pipeline, int srcOffset, int count)
{
  assert(srcOffset + count <= Count());
  assert(0 <= count && count <= pipeline.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      _store[ch][s + srcOffset] = pipeline[ch][s];
}

void 
FBPipelineAudioBlock::MoveOneRawBlockToAndPad(FBHostAudioBlock& raw)
{
  int samplesUsed = std::min(raw.Count(), Count());
  int samplesPadded = std::max(0, raw.Count() - samplesUsed);
  // TODO raw.SetToZero(0, samplesPadded);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
  {
    for (int s = samplesPadded; s < samplesPadded + samplesUsed; s++)
      raw._store[ch][s] = _store[ch][s - samplesPadded];
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + samplesUsed);
  }
}

void
FBPipelineAudioBlock::MoveOneFixedBlockTo(FBPlugAudioBlock& fixed)
{
  assert(_store[0].size() >= FB_PLUG_BLOCK_SIZE);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
  {
    for (int s = 0; s < FB_PLUG_BLOCK_SIZE; s++)
      fixed[ch][s] = _store[ch][s];
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + FB_PLUG_BLOCK_SIZE);
  }
}