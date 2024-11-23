#include <playground_base/dsp/FFFixedBlockProcessor.hpp>

FFFixedBlockProcessor::
FFFixedBlockProcessor(int maxHostSampleCount):
_fixedOutput(),
_inputSplitter(maxHostSampleCount),
_outputAggregator(maxHostSampleCount) {}