#include <playground_base/shared/FBHostBlock.hpp>

FBFixedInputBlock::
FBFixedInputBlock():
events(),
audio() {}

FBHostInputBlock::
FBHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}

FBHostEvents::
FBHostEvents():
FBPipelineHostEvents(),
blockParam()
{
  blockParam.reserve(FB_EVENT_COUNT_GUESS);
}

FBPipelineHostEvents::
FBPipelineHostEvents():
note(),
accParam()
{
  note.reserve(FB_EVENT_COUNT_GUESS);
  accParam.reserve(FB_EVENT_COUNT_GUESS);
}
