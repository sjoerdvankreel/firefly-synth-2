#include <playground_base/dsp/FBInputSplitter.hpp>
#include <algorithm>

template <class Event>
static void GatherAccEvents(
  std::vector<Event>& input, std::vector<Event>& output)
{
  int e = 0;
  output.clear();
  for (e = 0; e < input.size() && input[e].position < FB_FIXED_BLOCK_SIZE; e++)
    output.push_back(input[e]);
  input.erase(input.begin(), input.begin() + e);
  for (auto& e : input)
    e.position -= FB_FIXED_BLOCK_SIZE;
}

static void
GatherBlockParamEvents(
  std::vector<FBBlockParamEvent> const& input, std::vector<FBBlockParamEvent>& output)
{
  output.clear();
  output.insert(output.begin(), input.begin(), input.end());
}

FBInputSplitter::
FBInputSplitter(int maxHostSampleCount) :
_fixed(),
_accumulated(std::max(FB_FIXED_BLOCK_SIZE, maxHostSampleCount)),
_straddledAccParamEvents(FB_EVENT_COUNT_GUESS) {}

FBFixedInputBlock const*
FBInputSplitter::Split()
{
  if (_accumulated.sampleCount < FB_FIXED_BLOCK_SIZE)
    return nullptr;

  GatherAccEvents(_accumulated.events.note, _fixed.events.note);
  GatherAccEvents(_accumulated.events.accParam, _fixed.events.accParam);
  GatherBlockParamEvents(_accumulated.events.blockParam, _fixed.events.blockParam);

  _accumulated.audio.CopyTo(_fixed.audio, 0, 0, FB_FIXED_BLOCK_SIZE);
  _accumulated.audio.ShiftLeft(FB_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount -= FB_FIXED_BLOCK_SIZE;
  return &_fixed;
}

void 
FBInputSplitter::Accumulate(FBHostInputBlock const& input)
{
  _straddledAccParamEvents.clear();
  _straddledAccParamEvents.insert(
    _straddledAccParamEvents.begin(),
    input.events.accParam.begin(),
    input.events.accParam.end());
  auto compare = [](auto& l, auto& r) {
    return l.index == r.index ? l.position < r.position : l.index < r.index; };
  std::sort(_straddledAccParamEvents.begin(), _straddledAccParamEvents.end(), compare);
  for (int e = 0; e < _straddledAccParamEvents.size(); e++)
  {
    FBAccParamEvent thisEvent = _straddledAccParamEvents[e];
    thisEvent.position += _accumulated.sampleCount;
    _accumulated.events.accParam.push_back(thisEvent);
    if (e < _straddledAccParamEvents.size() - 1 &&
      _straddledAccParamEvents[e + 1].index == _straddledAccParamEvents[e].index)
    {
      FBAccParamEvent thatEvent = _straddledAccParamEvents[e + 1];
      thatEvent.position += _accumulated.sampleCount;
      int thisFixedBlock = thisEvent.position % FB_FIXED_BLOCK_SIZE;
      int thatFixedBlock = thatEvent.position % FB_FIXED_BLOCK_SIZE;
      for (int i = thisFixedBlock; i < thatFixedBlock; i++)
      {
        FBAccParamEvent straddledEvent;
        straddledEvent.index = thisEvent.index;
        straddledEvent.position = i * FB_FIXED_BLOCK_SIZE + FB_FIXED_BLOCK_SIZE - 1;
        straddledEvent.normalized = thisEvent.normalized + (thatEvent.normalized - thisEvent.normalized) * (straddledEvent.position / static_cast<float>(thatEvent.position));
        _accumulated.events.accParam.push_back(straddledEvent);
      }
    }
  }

  for (int e = 0; e < input.events.note.size(); e++)
  {
    FBNoteEvent event = input.events.note[e];
    event.position += _accumulated.sampleCount;
    _accumulated.events.note.push_back(event);
  }

  GatherBlockParamEvents(input.events.blockParam, _accumulated.events.blockParam);
  input.audio.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, input.audio.Count());
  _accumulated.sampleCount += input.audio.Count();
}