#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

FBFixedBlockProcessor::
FBFixedBlockProcessor(int maxHostSampleCount):
_fixedOutput(),
_inputSplitter(maxHostSampleCount),
_outputAggregator(maxHostSampleCount) {}