#include <playground_plug/base/FFInputSplitter.hpp>
#include <algorithm>

FFInputSplitter::
FFInputSplitter(int maxHostSampleCount):
_accumulatedBlock(std::max(FF_BLOCK_SIZE, maxHostSampleCount)) {}

FFHostInputBlock const*
FFInputSplitter::GetFirstFixedBlock() const
{
  if (_accumulatedBlock.sampleCount >= FF_BLOCK_SIZE)
    return &_accumulatedBlock;
  return nullptr;
}

void
FFInputSplitter::RemoveFirstFixedBlock()
{
  auto compare = [](auto const& e) { 
    return e.position < FF_BLOCK_SIZE; };

  std::erase_if(_accumulatedBlock.noteEvents, compare);
  for (auto& e : _accumulatedBlock.noteEvents)
    e.position -= FF_BLOCK_SIZE;

  std::erase_if(_accumulatedBlock.accParamEvents, compare);
  for(auto& e: _accumulatedBlock.accParamEvents)
    e.position -= FF_BLOCK_SIZE;

  _accumulatedBlock.sampleCount -= FF_BLOCK_SIZE;
  _accumulatedBlock.audio.ShiftLeft(FF_BLOCK_SIZE);
}

void 
FFInputSplitter::AccumulateHostBlock(
  FFHostInputBlock const& hostBlock)
{
  for (int event = 0; event < hostBlock.noteEvents.size(); event++)
  {
    FFNoteEvent noteEvent = hostBlock.noteEvents[event];
    noteEvent.position += _accumulatedBlock.sampleCount;
    _accumulatedBlock.noteEvents.push_back(noteEvent);
  }
  
  for (int event = 0; event < hostBlock.accParamEvents.size(); event++)
  {
    FFAccParamEvent accParamEvent = hostBlock.accParamEvents[event];
    accParamEvent.position += _accumulatedBlock.sampleCount;
    _accumulatedBlock.accParamEvents.push_back(accParamEvent);
  }

  for (int channel = 0; channel < FF_CHANNELS_STEREO; channel++)
    for (int sample = 0; sample < hostBlock.sampleCount; sample++)
    {
      float hostSample = hostBlock.audio[channel][sample];
      int accumulatedPos = _accumulatedBlock.sampleCount + sample;
      _accumulatedBlock.audio[channel][accumulatedPos] = hostSample;
    }

  _accumulatedBlock.sampleCount += hostBlock.sampleCount;
}