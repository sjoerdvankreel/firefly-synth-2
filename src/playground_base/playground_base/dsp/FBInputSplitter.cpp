#include <playground_base/dsp/FBInputSplitter.hpp>
#include <algorithm>

template <class Event>
static void GatherAccEvents(
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

static void
GatherBlockParamEvents(
  std::vector<FFBlockParamEvent> const& input, std::vector<FFBlockParamEvent>& output)
{
  output.clear();
  output.insert(output.begin(), input.begin(), input.end());
}

FBInputSplitter::
FBInputSplitter(int maxHostSampleCount):
_accumulated(std::max(FF_FIXED_BLOCK_SIZE, maxHostSampleCount)) {}

FBFixedInputBlock const*
FBInputSplitter::Split()
{
  if (_accumulated.sampleCount < FF_FIXED_BLOCK_SIZE)
    return nullptr;

  GatherAccEvents(_accumulated.events.note, _fixed.events.note);
  GatherAccEvents(_accumulated.events.accParam, _fixed.events.accParam);
  GatherBlockParamEvents(_accumulated.events.blockParam, _fixed.events.blockParam);

  _accumulated.audio.CopyTo(_fixed.audio, 0, 0, FF_FIXED_BLOCK_SIZE);
  _accumulated.audio.ShiftLeft(FF_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount -= FF_FIXED_BLOCK_SIZE;
  return &_fixed;
}

void 
FBInputSplitter::Accumulate(FFHostInputBlock const& input)
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

  GatherBlockParamEvents(input.events.blockParam, _accumulated.events.blockParam);
  input.audio.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, input.audio.Count());
  _accumulated.sampleCount += input.audio.Count();
}