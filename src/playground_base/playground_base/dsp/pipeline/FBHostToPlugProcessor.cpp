#include <playground_base/dsp/pipeline/FBHostToPlugProcessor.hpp>
#include <algorithm>

template <class Event>
static void GatherAccEvents(
  std::vector<Event>& input, std::vector<Event>& output)
{
  int e = 0;
  output.clear();
  for (e = 0; e < input.size() && input[e].position < FB_PLUG_BLOCK_SIZE; e++)
    output.push_back(input[e]);
  input.erase(input.begin(), input.begin() + e);
  for (auto& e : input)
    e.position -= FB_PLUG_BLOCK_SIZE;
}

static void
GatherStraddledAccParamEvents(
  FBAccParamEvent const& thisEvent,
  FBAccParamEvent const& nextEvent, 
  std::vector<FBAccParamEvent>& output)
{
  int thisFixedBlock = thisEvent.position / FB_PLUG_BLOCK_SIZE;
  int nextFixedBlock = nextEvent.position / FB_PLUG_BLOCK_SIZE;
  for (int i = thisFixedBlock; i < nextFixedBlock; i++)
  {
    FBAccParamEvent straddledEvent;
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
  _plug.audio.CopyFrom(_pipeline.audio);
  _pipeline.audio.Drop(_plug.audio.Count());
  GatherAccEvents(_pipeline.events.note, _plug.events.note);
  GatherAccEvents(_pipeline.events.accParam, _plug.events.accParam);
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

  for (int e = 0; e < input.events.accParam.size(); e++)
  {
    FBAccParamEvent thisEvent = input.events.accParam[e];
    thisEvent.position += _pipeline.audio.Count();
    _pipeline.events.accParam.push_back(thisEvent);
    if (e < input.events.accParam.size() - 1 &&
      input.events.accParam[e + 1].index == input.events.accParam[e].index)
    {
      FBAccParamEvent nextEvent = input.events.accParam[e + 1];
      nextEvent.position += _pipeline.audio.Count();
      GatherStraddledAccParamEvents(thisEvent, nextEvent, _pipeline.events.accParam);
    }
  }

  _pipeline.audio.Append(input.audio);
}