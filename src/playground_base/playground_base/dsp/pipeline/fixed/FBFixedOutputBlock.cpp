#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

FBFixedOutputBlock::
FBFixedOutputBlock(int runtimeParamCount)
{
  outputParamsNormalized.resize(runtimeParamCount);
}