#include <playground_plug/base/FFHostBlock.hpp>

FFHostInputBlock::
FFHostInputBlock(int maxSampleCount):
sampleCount(0),
audio(maxSampleCount),
noteEvents(),
accParamEvents(),
blockParamEvents() 
{
  noteEvents.reserve(FF_EVENT_COUNT_GUESS);
  accParamEvents.reserve(FF_EVENT_COUNT_GUESS);
  blockParamEvents.reserve(FF_EVENT_COUNT_GUESS);
}
