#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/buffer/FBHostToPlugProcessor.hpp>

#include <algorithm>

template <class Event>
static void GatherAccToFixed(
  std::vector<Event>& inputByParamThenSample, 
  std::vector<Event>& outputByParamThenSample)
{
  auto& input = inputByParamThenSample;
  auto& output = outputByParamThenSample;
  output.clear();
  for (int e = 0; e < input.size(); e++)
  {
    if (input[e].pos < FBFixedBlockSamples)
      output.push_back(input[e]);
    input[e].pos -= FBFixedBlockSamples;
  }
  std::erase_if(input, [](auto const& e) { return e.pos < 0; });
}

template <class Event>
static void GatherStraddledFromHost(
  Event const& thisEvent,
  Event const& nextEvent,
  std::vector<Event>& output)
{
  int thisFixedBlock = thisEvent.pos / FBFixedBlockSamples;
  int nextFixedBlock = nextEvent.pos / FBFixedBlockSamples;
  for (int i = thisFixedBlock; i < nextFixedBlock; i++)
  {
    Event straddledEvent = thisEvent;
    straddledEvent.pos = (i + 1) * FBFixedBlockSamples - 1;
    float valueRange = nextEvent.value - thisEvent.value;
    float normalizedPos = straddledEvent.pos / static_cast<float>(nextEvent.pos);
    straddledEvent.value = thisEvent.value + valueRange * normalizedPos;
    output.push_back(straddledEvent);
  }
}

template <class Event, class IsSameStream>
static void GatherAccFromHost(
  std::vector<Event> const& inputByParamThenSample,
  std::vector<Event>& outputByParamThenSample,
  IsSameStream isSameStream,
  int bufferOffset)
{
  auto& input = inputByParamThenSample;
  auto& output = outputByParamThenSample;
  for (int e = 0; e < input.size(); e++)
  {
    Event thisEvent = input[e];
    thisEvent.pos += bufferOffset;
    output.push_back(thisEvent);
    if (e < input.size() - 1 && isSameStream(input[e], input[e + 1]))
    {
      Event nextEvent = input[e + 1];
      nextEvent.pos += bufferOffset;
      GatherStraddledFromHost(thisEvent, nextEvent, output);
    }
  }
}

FBFixedInputBlock*
FBHostToPlugProcessor::ProcessToPlug()
{
  if (_buffer.audio.Count() < FBFixedBlockSamples)
    return nullptr;

  FBSArray2<float, FBFixedBlockSamples, 2> array = {};
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      _fixed.audio[ch].Set(s, _buffer.audio[ch][s]);
  _buffer.audio.Drop(FBFixedBlockSamples);
  GatherAccToFixed(_buffer.noteEvents, _fixed.noteEvents);
  GatherAccToFixed(_buffer.accAutoByParamThenSample, _fixed.accAutoByParamThenSample);
  GatherAccToFixed(_buffer.accModByParamThenNoteThenSample, _fixed.accModByParamThenNoteThenSample);
  return &_fixed;
}

void 
FBHostToPlugProcessor::BufferFromHost(FBHostInputBlock const& hostBlock)
{
  for (int e = 0; e < hostBlock.noteEvents.size(); e++)
  {
    FBNoteEvent event = hostBlock.noteEvents[e];
    event.pos += _buffer.audio.Count();
    _buffer.noteEvents.push_back(event);
  }

  GatherAccFromHost(
    hostBlock.accAutoByParamThenSample, _buffer.accAutoByParamThenSample,
    FBAccAutoEventIsSameStream, _buffer.audio.Count());
  GatherAccFromHost(
    hostBlock.accModByParamThenNoteThenSample, _buffer.accModByParamThenNoteThenSample,
    FBAccModEventIsSameStream, _buffer.audio.Count());
  _buffer.audio.AppendHostAudio(hostBlock.audio);
}