#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3AudioEffect.hpp>

#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugProcessor.hpp>
#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>

#include <cstring>
#include <algorithm>
#include <unordered_map>

static FBBlockEvent
MakeBlockEvent(int param, ParamValue value)
{
  FBBlockEvent result;
  result.param = param;
  result.normalized = value;
  return result;
}

static FBAccAutoEvent
MakeAccAutoEvent(int param, int pos, ParamValue value)
{
  FBAccAutoEvent result;
  result.pos = pos;
  result.param = param;
  result.value = value;
  return result;
}

static FBNoteEvent
MakeNoteOnEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = true;
  result.pos = event.sampleOffset;
  result.velo = event.noteOn.velocity;
  result.note.id = event.noteOn.noteId;
  result.note.key = event.noteOn.pitch;
  result.note.channel = event.noteOn.channel;
  return result;
}

static FBNoteEvent
MakeNoteOffEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = false;
  result.pos = event.sampleOffset;
  result.velo = event.noteOff.velocity;
  result.note.id = event.noteOff.noteId;
  result.note.key = event.noteOff.pitch;
  result.note.channel = event.noteOff.channel;
  return result;
}

FBVST3AudioEffect::
~FBVST3AudioEffect()
{
  FB_LOG_ENTRY_EXIT();
}

FBVST3AudioEffect::
FBVST3AudioEffect(
  FBStaticTopo const& topo, FUID const& controllerId):
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_procState(std::make_unique<FBProcStateContainer>(*_topo)),
_exchangeState(std::make_unique<FBExchangeStateContainer>(*_topo))
{
  FB_LOG_ENTRY_EXIT();
  setControllerClass(controllerId);
}

tresult PLUGIN_API 
FBVST3AudioEffect::setActive(TBool state)
{
  if (state)
    _exchangeHandler->onActivate(processSetup);
  else
    _exchangeHandler->onDeactivate();
  return AudioEffect::setActive(state);
}

tresult PLUGIN_API
FBVST3AudioEffect::getState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  std::string json = _topo->SaveProcStateToString(*_procState);
  if (!FBVST3SaveIBStream(state, json))
    return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API
FBVST3AudioEffect::setState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  std::string json;
  if (!FBVST3LoadIBStream(state, json))
    return kResultFalse;
  if (!_topo->LoadProcStateFromStringWithDryRun(json, *_procState))
    return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API
FBVST3AudioEffect::disconnect(IConnectionPoint* other)
{
  if (_exchangeHandler)
  {
    _exchangeHandler->onDisconnect(other);
    _exchangeHandler.reset();
  }
  return AudioEffect::disconnect(other);
}

tresult PLUGIN_API
FBVST3AudioEffect::initialize(FUnknown* context)
{
  FB_LOG_ENTRY_EXIT();
  if (AudioEffect::initialize(context) != kResultTrue)
    return kResultFalse;
  addEventInput(STR16("Event In"));
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3AudioEffect::canProcessSampleSize(int32 symbolicSize)
{
  if (symbolicSize == kSample32) 
    return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FBVST3AudioEffect::setBusArrangements(
  SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API
FBVST3AudioEffect::setupProcessing(ProcessSetup& setup)
{
  FB_LOG_ENTRY_EXIT();
  _sampleRate = (float)setup.sampleRate;
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(setup.maxSamplesPerBlock, 0.0f);
  _hostProcessor.reset(new FBHostProcessor(this));
  return kResultTrue;
}

tresult PLUGIN_API 
FBVST3AudioEffect::connect(IConnectionPoint* other)
{
  tresult result = AudioEffect::connect(other);
  if (result != kResultTrue)
    return result;
  auto callback = [this](DataExchangeHandler::Config& config, ProcessSetup const&) {
    config.numBlocks = 1;
    config.userContextID = 0;
    config.blockSize = _topo->static_.state.exchangeStateSize;
    config.alignment = _topo->static_.state.exchangeStateAlignment;
    return true; };
  _exchangeHandler = std::make_unique<DataExchangeHandler>(this, callback);
  _exchangeHandler->onConnect(other, getHostContext());
  return result;
}

tresult PLUGIN_API
FBVST3AudioEffect::process(ProcessData& data)
{
  Event event;
  _input.note.clear();
  if (data.inputEvents != nullptr)
    for (int i = 0; i < data.inputEvents->getEventCount(); i++)
      if (data.inputEvents->getEvent(i, event) == kResultOk)
        if (event.type == Event::kNoteOnEvent)
          _input.note.push_back(MakeNoteOnEvent(event));
        else if (event.type == Event::kNoteOffEvent)
          _input.note.push_back(MakeNoteOffEvent(event));

  int position;
  ParamValue value;
  IParamValueQueue* queue;
  std::unordered_map<int, int>::const_iterator iter;
  _input.block.clear();
  auto& accAuto = _input.accAutoByParamThenSample;
  accAuto.clear();
  if(data.inputParameterChanges != nullptr)
    for (int p = 0; p < data.inputParameterChanges->getParameterCount(); p++)
      if ((queue = data.inputParameterChanges->getParameterData(p)) != nullptr)
        if (queue->getPointCount() > 0)
          if ((iter = _topo->paramTagToIndex.find(queue->getParameterId())) != _topo->paramTagToIndex.end())
            if (_topo->params[iter->second].static_.acc)
            {
              for (int point = 0; point < queue->getPointCount(); point++)
                if (queue->getPoint(point, position, value) == kResultTrue)
                  accAuto.push_back(MakeAccAutoEvent(iter->second, position, value));
            }
            else
            {
              if (queue->getPoint(queue->getPointCount() - 1, position, value) == kResultTrue)
                _input.block.push_back(MakeBlockEvent(iter->second, value));
            }
  std::sort(accAuto.begin(), accAuto.end(), FBAccAutoEventOrderByParamThenPos);

  float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
  if (data.numInputs != 1)
    _input.audio = FBHostAudioBlock(zeroIn, data.numSamples);
  else
    _input.audio = FBHostAudioBlock(data.inputs[0].channelBuffers32, data.numSamples);
  _output.audio = FBHostAudioBlock(data.outputs->channelBuffers32, data.numSamples);

  _output.outputParams.clear();
  _hostProcessor->ProcessHost(_input, _output);

  if(data.outputParameterChanges != nullptr)
    for(int i = 0; i < _output.outputParams.size(); i++)
    {
      int unused;
      auto const& event = _output.outputParams[i];
      int tag = _topo->params[event.param].tag;
      auto queue = data.outputParameterChanges->addParameterData(tag, unused);
      if(queue != nullptr)
        queue->addPoint(0, event.normalized, unused);
    }

  if (_exchangeBlock.blockID == InvalidDataExchangeBlockID)
    _exchangeBlock = _exchangeHandler->getCurrentOrNewBlock();
  if (_exchangeBlock.blockID != InvalidDataExchangeBlockID)
  {
    memcpy(_exchangeBlock.data, _exchangeState->Raw(), _exchangeBlock.size);
    _exchangeHandler->sendCurrentBlock();
    _exchangeBlock = _exchangeHandler->getCurrentOrNewBlock();
  }

  return kResultTrue;
}