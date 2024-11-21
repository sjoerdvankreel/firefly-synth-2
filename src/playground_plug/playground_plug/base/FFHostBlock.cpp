#include <playground_plug/base/FFHostBlock.hpp>

FFHostBlock::
FFHostBlock(int maxSampleCount):
audioIn(maxSampleCount),
audioOut(maxSampleCount),
noteEvents(FF_EVENT_COUNT_GUESS, FFNoteEvent()),
accParamEvents(FF_EVENT_COUNT_GUESS, FFAccParamEvent()),
blockParamEvents(FF_EVENT_COUNT_GUESS, FFBlockParamEvent()) {}
