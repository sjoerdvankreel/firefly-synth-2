#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base_clap/FBCLAPExchangeStateQueue.hpp>

#include <playground_base/gui/shared/FBGUI.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/dsp/plug/FBPlugProcessor.hpp>
#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGUIStateContainer.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <clap/helpers/plugin.hxx>
#include <algorithm>

static FBBlockAutoEvent
MakeBlockAutoEvent(
  int param, double normalized)
{
  FBBlockAutoEvent result;
  result.param = param;
  result.normalized = static_cast<float>(normalized);
  return result;
}

static FBAccAutoEvent
MakeAccAutoEvent(
  int param, double normalized, int pos)
{
  FBAccAutoEvent result;
  result.pos = pos;
  result.param = param;
  result.value = static_cast<float>(normalized);
  return result;
}

static FBAccModEvent
MakeAccModEvent(
  int param, clap_event_param_mod const* event)
{
  FBAccModEvent result;
  result.param = param;
  result.pos = event->header.time;
  result.note.key = event->key;
  result.note.id = event->note_id;
  result.note.channel = event->channel;
  result.value = static_cast<float>(event->amount);
  return result;
}

static FBNoteEvent
MakeNoteEvent(
  clap_event_header_t const* header)
{
  FBNoteEvent result;
  auto event = reinterpret_cast<clap_event_note_t const*>(header);
  result.pos = header->time;
  result.note.key = event->key;
  result.note.id = event->note_id;
  result.note.channel = event->channel;
  result.velo = static_cast<float>(event->velocity);
  result.on = header->type == CLAP_EVENT_NOTE_ON;
  return result;
}

static clap_event_note 
MakeNoteEndEvent(FBNote const& note, int time)
{
  clap_event_note result = {};
  result.port_index = 0;
  result.velocity = 0.0f;
  result.key = note.key;
  result.note_id = note.id;
  result.channel = note.channel;
  result.header.flags = 0;
  result.header.time = time;
  result.header.type = CLAP_EVENT_NOTE_END;
  result.header.size = sizeof(clap_event_note);
  result.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
  return result;
}

static clap_event_param_value
MakeValueEvent(int paramTag, double value)
{
  clap_event_param_value result = {};
  result.value = value;
  result.param_id = paramTag;
  result.header.time = 0;
  result.header.flags = 0;
  result.header.type = CLAP_EVENT_PARAM_VALUE;
  result.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
  result.header.size = sizeof(clap_event_param_value);
  return result;
}

static clap_event_param_gesture
MakeGestureEvent(int paramTag, bool begin)
{
  clap_event_param_gesture result = {};
  result.param_id = paramTag;
  result.header.time = 0;
  result.header.flags = 0;
  result.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
  result.header.size = sizeof(clap_event_param_gesture);
  result.header.type = CLAP_EVENT_PARAM_GESTURE_BEGIN;
  if (!begin) result.header.type = CLAP_EVENT_PARAM_GESTURE_END;
  return result;
}

FBCLAPPlugin::
~FBCLAPPlugin() 
{
  FB_LOG_ENTRY_EXIT();
  stopTimer();
}

FBCLAPPlugin::
FBCLAPPlugin(
  FBStaticTopo const& topo,
  clap_plugin_descriptor const* desc,
  clap_host const* host,
  std::unique_ptr<FBCLAPExchangeStateQueueBase>&& exchangeStateQueue):
Plugin(desc, host),
_gui(),
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_guiState(std::make_unique<FBGUIStateContainer>(*_topo)),
_procState(std::make_unique<FBProcStateContainer>(*_topo)),
_editState(std::make_unique<FBScalarStateContainer>(*_topo)),
_dspExchangeState(std::make_unique<FBExchangeStateContainer>(*_topo)),
_guiExchangeState(std::make_unique<FBExchangeStateContainer>(*_topo)),
_exchangeStateQueue(std::move(exchangeStateQueue)),
_audioToMainEvents(FBCLAPSyncEventReserve - 1),
_mainToAudioEvents(FBCLAPSyncEventReserve - 1) 
{
  FB_LOG_ENTRY_EXIT();
}

bool
FBCLAPPlugin::init() noexcept
{
  FB_LOG_ENTRY_EXIT();
  startTimerHz(FBGUIFPS);
  return true;
}

void 
FBCLAPPlugin::timerCallback()
{
  FBCLAPSyncToMainEvent event = {};
  while (_audioToMainEvents.try_dequeue(event))
    if (_gui)
      _gui->SetAudioParamNormalizedFromHost(event.paramIndex, event.normalized);

  bool receivedExchange = false;
  while (_exchangeStateQueue->TryDequeue(_guiExchangeState->Raw()))
    receivedExchange = true;
  if (receivedExchange && _gui)
    _gui->UpdateExchangeState();
}

int32_t 
FBCLAPPlugin::getParamIndexForParamId(
  clap_id paramId) const noexcept
{
  auto iter = _topo->audio.paramTagToIndex.find(paramId);
  return iter == _topo->audio.paramTagToIndex.end() ? -1 : iter->second;
}

bool 
FBCLAPPlugin::getParamInfoForParamId(
  clap_id paramId, clap_param_info* info) const noexcept
{
  auto iter = _topo->audio.paramTagToIndex.find(paramId);
  if (iter == _topo->audio.paramTagToIndex.end())
    return false;
  return paramsInfo(iter->second, info);
}

bool
FBCLAPPlugin::isValidParamId(
  clap_id paramId) const noexcept
{
  return _topo->audio.paramTagToIndex.find(paramId) != _topo->audio.paramTagToIndex.end();
}

bool
FBCLAPPlugin::activate(
  double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept 
{
  FB_LOG_ENTRY_EXIT();
  _sampleRate = static_cast<float>(sampleRate);
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(maxFrameCount, 0.0f);
  _hostProcessor.reset(new FBHostProcessor(this));
  return true;
}

void 
FBCLAPPlugin::PushParamChangeToProcessorBlock(
  int index, double normalized, int pos)
{
  auto const& static_ = _topo->audio.params[index].static_;
  if (static_.acc)
    _input.accAutoByParamThenSample.push_back(MakeAccAutoEvent(index, normalized, pos));
  else
    _input.blockAuto.push_back(MakeBlockAutoEvent(index, normalized));
}

void 
FBCLAPPlugin::ProcessMainToAudioEvents(
  const clap_output_events* out, bool pushToProcBlock)
{
  FBCLAPSyncToAudioEvent uiEvent;
  while (_mainToAudioEvents.try_dequeue(uiEvent))
  {
    bool gestureBegin;
    clap_event_param_value valueToHost;
    clap_event_param_gesture gestureToHost;
    auto const& param = _topo->audio.params[uiEvent.paramIndex];
    switch (uiEvent.type)
    {
    case FBCLAPSyncEventType::EndChange:
    case FBCLAPSyncEventType::BeginChange:
      gestureBegin = uiEvent.type == FBCLAPSyncEventType::BeginChange;
      gestureToHost = MakeGestureEvent(param.tag, gestureBegin);
      out->try_push(out, &gestureToHost.header);
      break;
    case FBCLAPSyncEventType::PerformEdit:
      valueToHost = MakeValueEvent(param.tag, FBNormalizedToCLAP(param.static_, uiEvent.normalized));
      out->try_push(out, &valueToHost.header);
      if (pushToProcBlock)
        PushParamChangeToProcessorBlock(uiEvent.paramIndex, uiEvent.normalized, 0);
      else
        _procState->InitProcessing(uiEvent.paramIndex, uiEvent.normalized);
      break;
    default:
      assert(false);
      break;
    }
  }
}

clap_process_status 
FBCLAPPlugin::process(
  const clap_process* process) noexcept 
{
  _input.note.clear();
  _input.blockAuto.clear();
  _input.accAutoByParamThenSample.clear();
  _input.accModByParamThenNoteThenSample.clear();

  ProcessMainToAudioEvents(process->out_events, true);

  double normalized;
  auto inEvents = process->in_events;
  int inEventCount = inEvents->size(inEvents);
  clap_event_param_mod const* modFromHost;
  clap_event_param_value const* valueFromHost;
  std::unordered_map<int, int>::const_iterator iter;
  for (int i = 0; i < inEventCount; i++)
  {
    auto header = inEvents->get(inEvents, i);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID)
      continue;
    switch (header->type)
    {
    case CLAP_EVENT_NOTE_ON: 
    case CLAP_EVENT_NOTE_OFF: 
      _input.note.push_back(MakeNoteEvent(header)); 
      break;
    case CLAP_EVENT_PARAM_MOD:
      modFromHost = reinterpret_cast<clap_event_param_mod const*>(header);
      if ((iter = _topo->audio.paramTagToIndex.find(modFromHost->param_id)) != _topo->audio.paramTagToIndex.end())
        if (_topo->audio.params[iter->second].static_.acc)
          _input.accModByParamThenNoteThenSample.push_back(MakeAccModEvent(iter->second, modFromHost));
      break;
    case CLAP_EVENT_PARAM_VALUE:
      valueFromHost = reinterpret_cast<clap_event_param_value const*>(header);
      if ((iter = _topo->audio.paramTagToIndex.find(valueFromHost->param_id)) != _topo->audio.paramTagToIndex.end())
      {
        normalized = FBCLAPToNormalized(_topo->audio.params[iter->second].static_, valueFromHost->value);
        PushParamChangeToProcessorBlock(iter->second, normalized, valueFromHost->header.time);
        _audioToMainEvents.enqueue(FBMakeSyncToMainEvent(iter->second, normalized));
      }
      break;
    default:
      break;
    }
  }

  std::sort(
    _input.accAutoByParamThenSample.begin(),
    _input.accAutoByParamThenSample.end(),
    FBAccAutoEventOrderByParamThenPos);
  std::sort(
    _input.accModByParamThenNoteThenSample.begin(),
    _input.accModByParamThenNoteThenSample.end(),
    FBAccModEventOrderByParamThenNoteThenPos);

  _input.bpm = FBHostInputBlock::DefaultBPM;
  if (process->transport != nullptr)
    _input.bpm = static_cast<float>(process->transport->tempo);

  _output.outputParams.clear();
  _output.audio = FBHostAudioBlock(process->audio_outputs[0].data32, process->frames_count);

  float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
  if (process->audio_inputs_count != 1)
    _input.audio = FBHostAudioBlock(zeroIn, process->frames_count);
  else
    _input.audio = FBHostAudioBlock(process->audio_inputs[0].data32, process->frames_count);

  _hostProcessor->ProcessHost(_input, _output);
  _exchangeStateQueue->Enqueue(_dspExchangeState->Raw());

  for (auto const& op : _output.outputParams)
    _audioToMainEvents.enqueue(FBMakeSyncToMainEvent(op.param, op.normalized));

  auto const& rvs = _output.returnedVoices;
  for (int rv = 0; rv < rvs.size(); rv++)
  {
    clap_event_note endEvent = MakeNoteEndEvent(rvs[rv], process->frames_count - 1);
    process->out_events->try_push(process->out_events, &(endEvent.header));
  }
  return CLAP_PROCESS_CONTINUE;
}