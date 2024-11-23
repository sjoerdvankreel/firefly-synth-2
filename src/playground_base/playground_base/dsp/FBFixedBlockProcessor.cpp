#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

FFFixedBlockProcessor::
FFFixedBlockProcessor(int maxHostSampleCount):
_fixedOutput(),
_inputSplitter(maxHostSampleCount),
_outputAggregator(maxHostSampleCount) {}