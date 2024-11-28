#include <playground_base/shared/FBHostBlock.hpp>

FBHostInputBlock::
FBHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}

FBHostEvents::
FBHostEvents():
FBPipelineEvents(),
blockParam()
{
  blockParam.reserve(FB_EVENT_COUNT_GUESS);
}

FBPipelineEvents::
FBPipelineEvents():
note(),
accParam()
{
  note.reserve(FB_EVENT_COUNT_GUESS);
  accParam.reserve(FB_EVENT_COUNT_GUESS);
}
