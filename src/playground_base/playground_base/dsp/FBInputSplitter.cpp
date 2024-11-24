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

static void
GatherStraddlingAccParamEvents(
  std::vector<FBAccParamEvent> const& input, std::vector<FBAccParamEvent>& output)
{
  output.clear();
  output.insert(output.begin(), input.begin(), input.end());
  auto compare = [](auto& l, auto& r) {
    return l.index == r.index ? l.position < r.position : l.index < r.index; };
  std::sort(output.begin(), output.end(), compare);
}

static void
GatherStraddledAccParamEvents(
  FBAccParamEvent const& thisEvent,
  FBAccParamEvent const& nextEvent, 
  std::vector<FBAccParamEvent>& output)
{
  int thisFixedBlock = thisEvent.position / FB_FIXED_BLOCK_SIZE;
  int nextFixedBlock = nextEvent.position / FB_FIXED_BLOCK_SIZE;
  for (int i = thisFixedBlock; i < nextFixedBlock; i++)
  {
    FBAccParamEvent straddledEvent;
    straddledEvent.index = thisEvent.index;
    straddledEvent.position = i * FB_FIXED_BLOCK_SIZE + FB_FIXED_BLOCK_SIZE - 1;
    float valueRange = nextEvent.normalized - thisEvent.normalized;
    float normalizedPos = straddledEvent.position / static_cast<float>(nextEvent.position);
    straddledEvent.normalized = thisEvent.normalized + valueRange * normalizedPos;
    output.push_back(straddledEvent);
  }
}

FBInputSplitter::
FBInputSplitter(int maxHostSampleCount) :
_fixed(),
_accumulated(FB_FIXED_BLOCK_SIZE + maxHostSampleCount),
_straddledAccParamEvents()
{
  _straddledAccParamEvents.reserve(FB_EVENT_COUNT_GUESS);
}

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
  GatherBlockParamEvents(input.events.blockParam, _accumulated.events.blockParam);
  input.audio.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, input.audio.Count());

  for (int e = 0; e < input.events.note.size(); e++)
  {
    FBNoteEvent event = input.events.note[e];
    event.position += _accumulated.sampleCount;
    _accumulated.events.note.push_back(event);
  }

  GatherStraddlingAccParamEvents(input.events.accParam, _straddledAccParamEvents);
  for (int e = 0; e < _straddledAccParamEvents.size(); e++)
  {
    FBAccParamEvent thisEvent = _straddledAccParamEvents[e];
    thisEvent.position += _accumulated.sampleCount;
    _accumulated.events.accParam.push_back(thisEvent);
    if (e < _straddledAccParamEvents.size() - 1 &&
      _straddledAccParamEvents[e + 1].index == _straddledAccParamEvents[e].index)
    {
      FBAccParamEvent nextEvent = _straddledAccParamEvents[e + 1];
      nextEvent.position += _accumulated.sampleCount;
      GatherStraddledAccParamEvents(thisEvent, nextEvent, _accumulated.events.accParam);
    }
  }

  _accumulated.sampleCount += input.audio.Count();
}