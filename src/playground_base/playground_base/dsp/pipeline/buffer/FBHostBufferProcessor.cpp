#include <playground_base/dsp/pipeline/glue/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>

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
FBHostBufferProcessor::ProcessToFixed()
{
  if (_buffer.audio.Count() < FBFixedBlockSamples)
    return nullptr;

  FBFixedFloatAudioArray array = {};
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      array.data[ch].data[s] = _buffer.audio[ch][s];
  _fixed.audio.LoadFromFloatArray(array);
  _buffer.audio.Drop(FBFixedBlockSamples);
  GatherAccToFixed(_buffer.note, _fixed.note);
  GatherAccToFixed(_buffer.accAutoByParamThenSample, _fixed.accAutoByParamThenSample);
  GatherAccToFixed(_buffer.accModByParamThenNoteThenSample, _fixed.accModByParamThenNoteThenSample);
  return &_fixed;
}

void 
FBHostBufferProcessor::BufferFromHost(FBHostInputBlock const& input)
{
  for (int e = 0; e < input.note.size(); e++)
  {
    FBNoteEvent event = input.note[e];
    event.pos += _buffer.audio.Count();
    _buffer.note.push_back(event);
  }

  GatherAccFromHost(
    input.accAutoByParamThenSample, _buffer.accAutoByParamThenSample,
    FBAccAutoEventIsSameStream, _buffer.audio.Count());
  GatherAccFromHost(
    input.accModByParamThenNoteThenSample, _buffer.accModByParamThenNoteThenSample,
    FBAccModEventIsSameStream, _buffer.audio.Count());
  _buffer.audio.AppendHost(input.audio);
}