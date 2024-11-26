#include <playground_base/dsp/FBInputSplitter.hpp>
#include <algorithm>

static void
GatherBlockParamEvents(
  std::vector<FBBlockParamEvent> const& input,
  std::vector<FBBlockParamEvent>& output)
{
  output.clear();
  output.insert(output.begin(), input.begin(), input.end());
}

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
_accumulated(FB_FIXED_BLOCK_SIZE + maxHostSampleCount) {}

FBFixedInputBlock const*
FBInputSplitter::Split()
{
  if (_accumulated.audio.Count() < FB_FIXED_BLOCK_SIZE)
    return nullptr;

  _accumulated.audio.MoveOneFixedBlockTo(_fixed.audio);
  GatherAccEvents(_accumulated.events.note, _fixed.events.note);
  GatherAccEvents(_accumulated.events.accParam, _fixed.events.accParam);
  GatherBlockParamEvents(_accumulated.events.blockParam, _fixed.events.blockParam);
  return &_fixed;
}

void 
FBInputSplitter::Accumulate(FBHostInputBlock const& input)
{
  for (int e = 0; e < input.events.note.size(); e++)
  {
    FBNoteEvent event = input.events.note[e];
    event.position += _accumulated.audio.Count();
    _accumulated.events.note.push_back(event);
  }

  for (int e = 0; e < input.events.accParam.size(); e++)
  {
    FBAccParamEvent thisEvent = input.events.accParam[e];
    thisEvent.position += _accumulated.audio.Count();
    _accumulated.events.accParam.push_back(thisEvent);
    if (e < input.events.accParam.size() - 1 &&
      input.events.accParam[e + 1].index == input.events.accParam[e].index)
    {
      FBAccParamEvent nextEvent = input.events.accParam[e + 1];
      nextEvent.position += _accumulated.audio.Count();
      GatherStraddledAccParamEvents(thisEvent, nextEvent, _accumulated.events.accParam);
    }
  }

  GatherBlockParamEvents(input.events.blockParam, _accumulated.events.blockParam);
  _accumulated.audio.AppendFrom(input.audio);
}