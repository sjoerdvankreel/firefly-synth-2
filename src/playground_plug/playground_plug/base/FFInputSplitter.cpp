#include <playground_plug/base/FFInputSplitter.hpp>
#include <algorithm>

FFInputSplitter::
FFInputSplitter(int maxHostSampleCount):
_accumulating(std::max(FF_BLOCK_SIZE, maxHostSampleCount)) {}

FFAccumulatingInputBlock const*
FFInputSplitter::GetFirstFixedBlock() const
{
  if (_accumulating.sampleCount >= FF_BLOCK_SIZE)
    return &_accumulating;
  return nullptr;
}

void
FFInputSplitter::RemoveFirstFixedBlock()
{
  auto compare = [](auto const& e) { 
    return e.position < FF_BLOCK_SIZE; };

  std::erase_if(_accumulating.events.note, compare);
  for (auto& e : _accumulating.events.note)
    e.position -= FF_BLOCK_SIZE;

  std::erase_if(_accumulating.events.accParam, compare);
  for(auto& e: _accumulating.events.accParam)
    e.position -= FF_BLOCK_SIZE;

  _accumulating.sampleCount -= FF_BLOCK_SIZE;
  _accumulating.audio.ShiftLeft(FF_BLOCK_SIZE);
}

void 
FFInputSplitter::AccumulateHostBlock(
  FFHostInputBlock const& host)
{
  for (int e = 0; e < host.events.note.size(); e++)
  {
    FFNoteEvent event = host.events.note[e];
    event.position += _accumulating.sampleCount;
    _accumulating.events.note.push_back(event);
  }

  for (int e = 0; e < host.events.accParam.size(); e++)
  {
    FFAccParamEvent event = host.events.accParam[e];
    event.position += _accumulating.sampleCount;
    _accumulating.events.accParam.push_back(event);
  }

  for (int c = 0; c < FF_CHANNELS_STEREO; c++)
    for (int s = 0; s < host.audio.Count(); s++)
    {
      float sample = host.audio[c][s];
      int accumulatedPos = _accumulating.sampleCount + s;
      _accumulating.audio[c][accumulatedPos] = sample;
    }

  _accumulating.sampleCount += host.audio.Count();
}