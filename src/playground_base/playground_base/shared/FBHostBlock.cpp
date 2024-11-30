#include <playground_base/shared/FBHostBlock.hpp>

FBHostInputBlock::
FBHostInputBlock(float* l, float* r, int count):
events(),
audio(l, r, count) {}
