#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/FBHostToPlugProcessor.hpp>
#include <algorithm>

// TODO alot

template <class Event>
static void GatherAcc(
  std::vector<Event>& input, std::vector<Event>& output)
{
  int e = 0;
  output.clear();
  for (e = 0; e < input.size() && input[e].position < FBPlugAudioBlock::Count(); e++)
    output.push_back(input[e]);
  input.erase(input.begin(), input.begin() + e);
  for (auto& e : input)
    e.position -= FBPlugAudioBlock::Count();
}

static void
GatherStraddledEvents(
  FBAccEvent const& thisEvent,
  FBAccEvent const& nextEvent, 
  std::vector<FBAccEvent>& output)
{
  int thisFixedBlock = thisEvent.position / FB_PLUG_BLOCK_SIZE;
  int nextFixedBlock = nextEvent.position / FB_PLUG_BLOCK_SIZE;
  for (int i = thisFixedBlock; i < nextFixedBlock; i++)
  {
    FBAccEvent straddledEvent;
    straddledEvent.index = thisEvent.index;
    straddledEvent.position = i * FB_PLUG_BLOCK_SIZE + FB_PLUG_BLOCK_SIZE - 1;
    float valueRange = nextEvent.normalized - thisEvent.normalized;
    float normalizedPos = straddledEvent.position / static_cast<float>(nextEvent.position);
    straddledEvent.normalized = thisEvent.normalized + valueRange * normalizedPos;
    output.push_back(straddledEvent);
  }
}

FBPlugInputBlock const*
FBHostToPlugProcessor::ToPlug()
{
  if (_pipeline.audio.Count() < FB_PLUG_BLOCK_SIZE)
    return nullptr;
  _plug.audio.CopyFrom(_pipeline.audio, 0, 0, _plug.audio.Count());
  _pipeline.audio.Drop(_plug.audio.Count());
  GatherAcc(_pipeline.events.note, _plug.events.note);
  GatherAcc(_pipeline.events.acc, _plug.events.acc);
  return &_plug;
}

void 
FBHostToPlugProcessor::FromHost(FBHostInputBlock const& input)
{
  for (int e = 0; e < input.events.note.size(); e++)
  {
    FBNoteEvent event = input.events.note[e];
    event.position += _pipeline.audio.Count();
    _pipeline.events.note.push_back(event);
  }

  for (int e = 0; e < input.events.acc.size(); e++)
  {
    FBAccEvent thisEvent = input.events.acc[e];
    thisEvent.position += _pipeline.audio.Count();
    _pipeline.events.acc.push_back(thisEvent);
    if (e < input.events.acc.size() - 1 &&
      input.events.acc[e + 1].index == input.events.acc[e].index)
    {
      FBAccEvent nextEvent = input.events.acc[e + 1];
      nextEvent.position += _pipeline.audio.Count();
      GatherStraddledEvents(thisEvent, nextEvent, _pipeline.events.acc);
    }
  }

  _pipeline.audio.Append(input.audio);
}