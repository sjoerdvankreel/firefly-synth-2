#include <playground_plug/base/FFHostBlock.hpp>

FFHostEvents::
FFHostEvents():
note(),
accParam(),
blockParam()
{
  note.reserve(FF_EVENT_COUNT_GUESS);
  accParam.reserve(FF_EVENT_COUNT_GUESS);
  blockParam.reserve(FF_EVENT_COUNT_GUESS);
}

FFAccumulatingOutputBlock::
FFAccumulatingOutputBlock(int maxSampleCount):
sampleCount(0),
audio(maxSampleCount) {}

FFAccumulatingInputBlock::
FFAccumulatingInputBlock(int maxSampleCount):
sampleCount(0),
events(),
audio(maxSampleCount) {}

FFHostOutputBlock::
FFHostOutputBlock(float* l, float* r, int count) :
audio(l, r, count) {}

FFHostInputBlock::
FFHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}
