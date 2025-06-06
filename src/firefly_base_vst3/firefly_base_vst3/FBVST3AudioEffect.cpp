#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3AudioEffect.hpp>

#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/dsp/plug/FBPlugProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>

#include <cstring>
#include <algorithm>
#include <unordered_map>

static FBBlockAutoEvent
MakeBlockAutoEvent(int param, ParamValue value)
{
  FBBlockAutoEvent result;
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
  processContextRequirements.needTempo();
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
  _topo->LoadProcStateFromStringWithDryRun(json, *_procState);
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
  _sampleRate = static_cast<float>(setup.sampleRate);
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
    config.blockSize = _topo->static_.exchangeStateSize;
    config.alignment = _topo->static_.exchangeStateAlignment;
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
  _input.blockAuto.clear();
  auto& accAuto = _input.accAutoByParamThenSample;
  accAuto.clear();
  if(data.inputParameterChanges != nullptr)
    for (int p = 0; p < data.inputParameterChanges->getParameterCount(); p++)
      if ((queue = data.inputParameterChanges->getParameterData(p)) != nullptr)
        if (queue->getPointCount() > 0)
          if ((iter = _topo->audio.paramTagToIndex.find(queue->getParameterId())) != _topo->audio.paramTagToIndex.end())
            if (_topo->audio.params[iter->second].static_.acc)
            {
              for (int point = 0; point < queue->getPointCount(); point++)
                if (queue->getPoint(point, position, value) == kResultTrue)
                  accAuto.push_back(MakeAccAutoEvent(iter->second, position, value));
            }
            else
            {
              if (queue->getPoint(queue->getPointCount() - 1, position, value) == kResultTrue)
                _input.blockAuto.push_back(MakeBlockAutoEvent(iter->second, value));
            }
  std::sort(accAuto.begin(), accAuto.end(), FBAccAutoEventOrderByParamThenPos);

  _output.outputParams.clear();
  _output.audio = FBHostAudioBlock(data.outputs->channelBuffers32, data.numSamples);

  _input.bpm = FBHostInputBlock::DefaultBPM;
  if (data.processContext != nullptr && (data.processContext->state & ProcessContext::kTempoValid) != 0)
    _input.bpm = static_cast<float>(data.processContext->tempo);

  float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
  if (data.numInputs != 1)
    _input.audio = FBHostAudioBlock(zeroIn, data.numSamples);
  else
    _input.audio = FBHostAudioBlock(data.inputs[0].channelBuffers32, data.numSamples);

  _hostProcessor->ProcessHost(_input, _output);

  if(data.outputParameterChanges != nullptr)
    for(int i = 0; i < _output.outputParams.size(); i++)
    {
      int unused;
      auto const& event = _output.outputParams[i];
      int tag = _topo->audio.params[event.param].tag;
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