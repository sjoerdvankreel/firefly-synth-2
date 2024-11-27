#include <playground_base/shared/FBHostBlock.hpp>

FBFixedInputBlock::
FBFixedInputBlock():
events(),
audio() {}

FBAccumulatedInputBlock::
FBAccumulatedInputBlock(int maxSampleCount):
events(),
audio(maxSampleCount) {}

FBHostInputBlock::
FBHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}

FBHostEvents::
FBHostEvents():
FBAccumulatedHostEvents(),
blockParam()
{
  blockParam.reserve(FB_EVENT_COUNT_GUESS);
}

FBAccumulatedHostEvents::
FBAccumulatedHostEvents():
note(),
accParam()
{
  note.reserve(FB_EVENT_COUNT_GUESS);
  accParam.reserve(FB_EVENT_COUNT_GUESS);
}
