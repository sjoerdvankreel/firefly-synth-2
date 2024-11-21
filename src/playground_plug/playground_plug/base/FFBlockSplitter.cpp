#include <playground_plug/base/FFBlockSplitter.hpp>
#include <algorithm>

FFBlockSplitter::
FFBlockSplitter(int maxHostSampleCount):
_accumulatedSampleCount(0),
_accumulatedBlock(std::max(FF_BLOCK_SIZE, maxHostSampleCount)) {}

bool
FFBlockSplitter::GetCurrentFixedBlock(
  FFHostBlock const* fixedBlock)
{
  fixedBlock = &_accumulatedBlock;
  return _accumulatedSampleCount >= FF_BLOCK_SIZE;
}

void
FFBlockSplitter::RemoveOneFixedBlock()
{
  auto compare = [](auto const& e) { 
    return e.position < FF_BLOCK_SIZE; };
  
  std::erase_if(_accumulatedBlock.noteEvents, compare);
  for (auto& e : _accumulatedBlock.noteEvents)
    e.position -= FF_BLOCK_SIZE;

  std::erase_if(_accumulatedBlock.accParamEvents, compare);
  for(auto& e: _accumulatedBlock.accParamEvents)
    e.position -= FF_BLOCK_SIZE;

  _accumulatedBlock.audioIn.ShiftLeft(FF_BLOCK_SIZE);
  _accumulatedSampleCount -= FF_BLOCK_SIZE;
}

void 
FFBlockSplitter::Accumulate(
  FFHostBlock const& hostBlock, int currentHostSampleCount)
{
  for (int event = 0; event < hostBlock.noteEvents.size(); event++)
  {
    FFNoteEvent noteEvent = hostBlock.noteEvents[event];
    noteEvent.position += _accumulatedSampleCount;
    _accumulatedBlock.noteEvents.push_back(noteEvent);
  }
  
  for (int event = 0; event < hostBlock.accParamEvents.size(); event++)
  {
    FFAccParamEvent accParamEvent = hostBlock.accParamEvents[event];
    accParamEvent.position += _accumulatedSampleCount;
    _accumulatedBlock.accParamEvents.push_back(accParamEvent);
  }

  for (int channel = 0; channel < FF_CHANNELS_STEREO; channel++)
    for (int sample = 0; sample < currentHostSampleCount; sample++)
    {
      float hostSample = hostBlock.audioIn[channel][sample];
      int accumulatedPos = _accumulatedSampleCount + sample;
      _accumulatedBlock.audioIn[channel][accumulatedPos] = hostSample;
    }
}