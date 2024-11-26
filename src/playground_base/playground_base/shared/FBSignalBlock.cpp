#include <playground_base/shared/FBSignalBlock.hpp>
#include <cassert>

FBDynamicAudioBlock::
FBDynamicAudioBlock(int capacity):
_store()
{
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    _store[ch].reserve(capacity);
}

void
FBRawAudioBlockView::SetToZero(int from, int to)
{
  assert(0 <= from && from <= to && to <= _count);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    for (int i = from; i < to; i++)
      _store[ch][i] = 0.0f;
}

void 
FBDynamicAudioBlock::AppendFrom(FBFixedAudioBlock const& fixed)
{
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    for (int s = 0; s < FB_FIXED_BLOCK_SIZE; s++)
      _store[ch].push_back(fixed[ch][s]);
}

void
FBDynamicAudioBlock::AppendFrom(FBRawAudioBlockView const& raw)
{
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    for (int s = 0; s < raw.Count(); s++)
      _store[ch].push_back(raw._store[ch][s]);
}

void 
FBDynamicAudioBlock::MoveOneRawBlockToAndPad(FBRawAudioBlockView& raw)
{
  int samplesUsed = std::min(raw.Count(), Count());
  int samplesPadded = std::max(0, raw.Count() - samplesUsed);
  raw.SetToZero(0, samplesPadded);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
  {
    for (int s = samplesPadded; s < samplesPadded + samplesUsed; s++)
      raw._store[ch][s] = _store[ch][s - samplesPadded];
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + samplesUsed);
  }
}

void
FBDynamicAudioBlock::MoveOneFixedBlockTo(FBFixedAudioBlock& fixed)
{
  assert(_store[0].size() >= FB_FIXED_BLOCK_SIZE);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
  {
    for (int s = 0; s < FB_FIXED_BLOCK_SIZE; s++)
      fixed[ch][s] = _store[ch][s];
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + FB_FIXED_BLOCK_SIZE);
  }
}