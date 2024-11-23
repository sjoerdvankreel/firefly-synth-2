#include <playground_plug/base/FFInputSplitter.hpp>
#include <algorithm>

template <class Event>
static void GatherEvents(
  std::vector<Event>& input, 
  std::vector<Event>& output)
{
  // TODO handle straddle fixed boundary

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
FFInputSplitter::NextFixedBlock()
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
FFInputSplitter::AccumulateHostBlock(
  FFHostInputBlock const& host)
{
  for (int e = 0; e < host.events.note.size(); e++)
  {
    FFNoteEvent event = host.events.note[e];
    event.position += _accumulated.sampleCount;
    _accumulated.events.note.push_back(event);
  }

  for (int e = 0; e < host.events.accParam.size(); e++)
  {
    FFAccParamEvent event = host.events.accParam[e];
    event.position += _accumulated.sampleCount;
    _accumulated.events.accParam.push_back(event);
  }

  host.audio.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, host.audio.Count());
  _accumulated.sampleCount += host.audio.Count();
}