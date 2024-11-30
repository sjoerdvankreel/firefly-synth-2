#include <playground_base/shared/FBHostBlock.hpp>

FBHostInputBlock::
FBHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}

FBHostEvents::
FBHostEvents():
FBPipelineEvents(),
block()
{
  block.reserve(FB_EVENT_COUNT_GUESS);
}

FBPipelineEvents::
FBPipelineEvents():
note(),
acc()
{
  note.reserve(FB_EVENT_COUNT_GUESS);
  acc.reserve(FB_EVENT_COUNT_GUESS);
}
