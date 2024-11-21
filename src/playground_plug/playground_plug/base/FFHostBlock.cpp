#include <playground_plug/base/FFHostBlock.hpp>

FFHostBlock::
FFHostBlock(int maxSampleCount):
audioIn(maxSampleCount),
audioOut(maxSampleCount),
noteEvents(),
accParamEvents(),
blockParamEvents() 
{
  noteEvents.reserve(FF_EVENT_COUNT_GUESS);
  accParamEvents.reserve(FF_EVENT_COUNT_GUESS);
  blockParamEvents.reserve(FF_EVENT_COUNT_GUESS);
}
