#include <playground_base/shared/FBHostBlock.hpp>

FBHostEvents::
FBHostEvents():
note(),
accParam(),
blockParam()
{
  note.reserve(FB_EVENT_COUNT_GUESS);
  accParam.reserve(FB_EVENT_COUNT_GUESS);
  blockParam.reserve(FB_EVENT_COUNT_GUESS);
}

FBFixedInputBlock::
FBFixedInputBlock():
events(),
audio() {}

FBAccumulatedOutputBlock::
FBAccumulatedOutputBlock(int maxSampleCount):
sampleCount(0),
audio(maxSampleCount) {}

FBAccumulatedInputBlock::
FBAccumulatedInputBlock(int maxSampleCount):
sampleCount(0),
events(),
audio(maxSampleCount) {}

FBHostInputBlock::
FBHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}
