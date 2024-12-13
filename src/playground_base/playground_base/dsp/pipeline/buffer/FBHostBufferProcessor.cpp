#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>

#include <algorithm>

template <class Event>
static void GatherAcc(
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

// TODO mod
static void
GatherStraddledEvents(
  FBAccAutoEvent const& thisEvent,
  FBAccAutoEvent const& nextEvent,
  std::vector<FBAccAutoEvent>& output)
{
  int thisFixedBlock = thisEvent.pos / FBFixedBlockSamples;
  int nextFixedBlock = nextEvent.pos / FBFixedBlockSamples;
  for (int i = thisFixedBlock; i < nextFixedBlock; i++)
  {
    FBAccAutoEvent straddledEvent;
    straddledEvent.index = thisEvent.index;
    straddledEvent.pos = (i + 1) * FBFixedBlockSamples - 1;
    float valueRange = nextEvent.normalized - thisEvent.normalized;
    float normalizedPos = straddledEvent.pos / static_cast<float>(nextEvent.pos);
    straddledEvent.normalized = thisEvent.normalized + valueRange * normalizedPos;
    output.push_back(straddledEvent);
  }
}

FBFixedInputBlock const*
FBHostBufferProcessor::ProcessToFixed()
{
  if (_buffer.audio.Count() < FBFixedBlockSamples)
    return nullptr;

  _fixed.audio.CopyFrom(_buffer.audio);
  _buffer.audio.Drop(FBFixedBlockSamples);
  GatherAcc(_buffer.note, _fixed.note);
  GatherAcc(_buffer.accAutoByParamThenSample, _fixed.accAutoByParamThenSample); // TODO mod
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

  // TODO mod
  auto const& accAuto = input.accAutoByParamThenSample;
  for (int e = 0; e < accAuto.size(); e++)
  {
    FBAccAutoEvent thisEvent = accAuto[e];
    thisEvent.pos += _buffer.audio.Count();
    _buffer.accAutoByParamThenSample.push_back(thisEvent);
    if (e < accAuto.size() - 1 &&
      accAuto[e + 1].index == accAuto[e].index)
    {
      FBAccAutoEvent nextEvent = accAuto[e + 1];
      nextEvent.pos += _buffer.audio.Count();
      GatherStraddledEvents(thisEvent, nextEvent, _buffer.accAutoByParamThenSample);
    }
  }

  _buffer.audio.Append(input.audio);
}