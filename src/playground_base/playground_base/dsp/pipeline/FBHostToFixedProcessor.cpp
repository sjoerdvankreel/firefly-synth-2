#include <playground_base/dsp/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/FBHostToFixedProcessor.hpp>
#include <algorithm>

template <class Event>
static void GatherAcc(
  std::vector<Event>& input, std::vector<Event>& output)
{
  int e = 0;
  output.clear();
  for (e = 0; e < input.size() && input[e].pos < FBFixedAudioBlock::Count(); e++)
    output.push_back(input[e]);
  input.erase(input.begin(), input.begin() + e);
  for (auto& e : input)
    e.pos -= FBFixedAudioBlock::Count();
}

static void
GatherStraddledEvents(
  FBAccEvent const& thisEvent,
  FBAccEvent const& nextEvent, 
  std::vector<FBAccEvent>& output)
{
  int thisFixedBlock = thisEvent.pos / FBFixedAudioBlock::Count();
  int nextFixedBlock = nextEvent.pos / FBFixedAudioBlock::Count();
  for (int i = thisFixedBlock; i < nextFixedBlock; i++)
  {
    FBAccEvent straddledEvent;
    straddledEvent.index = thisEvent.index;
    straddledEvent.pos = i * FBFixedAudioBlock::Count() + FBFixedAudioBlock::Count() - 1;
    float valueRange = nextEvent.normalized - thisEvent.normalized;
    float normalizedPos = straddledEvent.pos / static_cast<float>(nextEvent.pos);
    straddledEvent.normalized = thisEvent.normalized + valueRange * normalizedPos;
    output.push_back(straddledEvent);
  }
}

FBFixedInputBlock const*
FBHostToFixedProcessor::ToFixed()
{
  if (_pipeline.audio.Count() < FBFixedAudioBlock::Count())
    return nullptr;
  _fixed.audio.CopyFrom(_pipeline.audio, 0, 0, FBFixedAudioBlock::Count());
  _pipeline.audio.Drop(FBFixedAudioBlock::Count());
  GatherAcc(_pipeline.note, _fixed.note);
  GatherAcc(_pipeline.acc, _fixed.acc);
  return &_fixed;
}

void 
FBHostToFixedProcessor::FromHost(FBHostInputBlock const& input)
{
  for (int e = 0; e < input.note.size(); e++)
  {
    FBNoteEvent event = input.note[e];
    event.pos += _pipeline.audio.Count();
    _pipeline.note.push_back(event);
  }

  for (int e = 0; e < input.acc.size(); e++)
  {
    FBAccEvent thisEvent = input.acc[e];
    thisEvent.pos += _pipeline.audio.Count();
    _pipeline.acc.push_back(thisEvent);
    if (e < input.acc.size() - 1 &&
      input.acc[e + 1].index == input.acc[e].index)
    {
      FBAccEvent nextEvent = input.acc[e + 1];
      nextEvent.pos += _pipeline.audio.Count();
      GatherStraddledEvents(thisEvent, nextEvent, _pipeline.acc);
    }
  }

  _pipeline.audio.Append(input.audio);
}