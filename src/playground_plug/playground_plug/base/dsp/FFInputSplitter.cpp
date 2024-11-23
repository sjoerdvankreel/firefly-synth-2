#include <playground_plug/base/dsp/FFInputSplitter.hpp>
#include <algorithm>

template <class Event>
static void GatherEvents(
  std::vector<Event>& input, std::vector<Event>& output)
{
  int e = 0;
  output.clear();
  for (e = 0; e < input.size() && input[e].position < FF_FIXED_BLOCK_SIZE; e++)
    output.push_back(input[e]);
  input.erase(input.begin(), input.begin() + e);
  for (auto& e : input)
    e.position -= FF_FIXED_BLOCK_SIZE;
}

FFInputSplitter::
FFInputSplitter(int maxHostSampleCount):
_accumulated(std::max(FF_FIXED_BLOCK_SIZE, maxHostSampleCount)) {}

FFFixedInputBlock const*
FFInputSplitter::Split()
{
  if (_accumulated.sampleCount < FF_FIXED_BLOCK_SIZE)
    return nullptr;

  GatherEvents(_accumulated.events.note, _fixed.events.note);
  GatherEvents(_accumulated.events.accParam, _fixed.events.accParam);

  _accumulated.audio.CopyTo(_fixed.audio, 0, 0, FF_FIXED_BLOCK_SIZE);
  _accumulated.audio.ShiftLeft(FF_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount -= FF_FIXED_BLOCK_SIZE;
  return &_fixed;
}

void 
FFInputSplitter::Accumulate(FFHostInputBlock const& input)
{
  for (int e = 0; e < input.events.note.size(); e++)
  {
    FFNoteEvent event = input.events.note[e];
    event.position += _accumulated.sampleCount;
    _accumulated.events.note.push_back(event);
  }

  for (int e = 0; e < input.events.accParam.size(); e++)
  {
    FFAccParamEvent event = input.events.accParam[e];
    event.position += _accumulated.sampleCount;
    _accumulated.events.accParam.push_back(event);
  }

  input.audio.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, input.audio.Count());
  _accumulated.sampleCount += input.audio.Count();
}