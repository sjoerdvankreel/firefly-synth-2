#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

#include <clap/helpers/plugin.hxx>
#include <algorithm>

static FBBlockEvent
MakeBlockEvent(
  int param, double normalized)
{
  FBBlockEvent result;
  result.param = param;
  result.normalized = (float)normalized;
  return result;
}

static FBAccAutoEvent
MakeAccAutoEvent(
  int param, double normalized, int pos)
{
  FBAccAutoEvent result;
  result.pos = pos;
  result.param = param;
  result.value = (float)normalized;
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
  result.value = (float)event->amount;
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
  result.velo = (float)event->velocity;
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
  stopTimer();
}

FBCLAPPlugin::
FBCLAPPlugin(
  FBStaticTopo const& topo,
  clap_plugin_descriptor const* desc, 
  clap_host const* host):
Plugin(desc, host),
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_procState(*_topo),
_guiState(*_topo),
_gui(),
_audioToMainEvents(FBCLAPSyncEventReserve - 1),
_mainToAudioEvents(FBCLAPSyncEventReserve - 1) {}

float
FBCLAPPlugin::GetParamNormalized(int index) const
{
  return *_guiState.Params()[index];
}

void 
FBCLAPPlugin::ProcessVoices()
{
  // todo maybe drop this ?
  if (!_host.canUseThreadPool() ||
    !_host.threadPoolRequestExec(FBMaxVoices))
    _hostProcessor->ProcessAllVoices();
}

bool
FBCLAPPlugin::init() noexcept
{
  startTimerHz(60); // TODO 60?
  return true;
}

void 
FBCLAPPlugin::timerCallback()
{
  FBCLAPSyncToMainEvent event;
  while (_audioToMainEvents.try_dequeue(event))
    if (_gui)
      _gui->SetParamNormalized(event.paramIndex, event.normalized);
}

void 
FBCLAPPlugin::EndParamChange(int index)
{
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::EndChange, index, 0.0f);
  _mainToAudioEvents.enqueue(event);
  if(_host.canUseParams())
    _host.paramsRequestFlush();
}

void 
FBCLAPPlugin::BeginParamChange(int index)
{
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::BeginChange, index, 0.0f);
  _mainToAudioEvents.enqueue(event);
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

void 
FBCLAPPlugin::PerformParamEdit(int index, float normalized)
{  
  auto event = FBMakeSyncToAudioEvent(FBCLAPSyncEventType::PerformEdit, index, normalized);
  _mainToAudioEvents.enqueue(event);
  *_guiState.Params()[index] = normalized;
  if (_host.canUseParams())
    _host.paramsRequestFlush();
}

int32_t 
FBCLAPPlugin::getParamIndexForParamId(
  clap_id paramId) const noexcept
{
  auto iter = _topo->paramTagToIndex.find(paramId);
  return iter == _topo->paramTagToIndex.end() ? -1 : iter->second;
}

bool 
FBCLAPPlugin::getParamInfoForParamId(
  clap_id paramId, clap_param_info* info) const noexcept
{
  auto iter = _topo->paramTagToIndex.find(paramId);
  if (iter == _topo->paramTagToIndex.end())
    return false;
  return paramsInfo(iter->second, info);
}

bool
FBCLAPPlugin::isValidParamId(
  clap_id paramId) const noexcept
{
  return _topo->paramTagToIndex.find(paramId) != _topo->paramTagToIndex.end();
}

bool
FBCLAPPlugin::activate(
  double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept 
{
  float fSampleRate = (float)sampleRate;
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(maxFrameCount, 0.0f);
  auto plug = MakePlugProcessor(_topo->static_, _procState.Raw(), fSampleRate);
  _hostProcessor.reset(new FBHostProcessor(this, _topo->static_, std::move(plug), &_procState, fSampleRate));
  return true;
}

void 
FBCLAPPlugin::PushParamChangeToProcessorBlock(
  int index, double normalized, int pos)
{
  auto const& static_ = _topo->params[index].static_;
  if (static_.acc)
    _input.accAutoByParamThenSample.push_back(MakeAccAutoEvent(index, normalized, pos));
  else
    _input.block.push_back(MakeBlockEvent(index, normalized));
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
    auto const& param = _topo->params[uiEvent.paramIndex];
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
        _procState.InitProcessing(uiEvent.paramIndex, uiEvent.normalized);
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
  _input.block.clear();
  _input.accAutoByParamThenSample.clear();
  _input.accModByParamThenNoteThenSample.clear();

  ProcessMainToAudioEvents(process->out_events, true);

  double normalized;
  FBCLAPSyncToMainEvent syncToMain;
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
      if ((iter = _topo->paramTagToIndex.find(modFromHost->param_id)) != _topo->paramTagToIndex.end())
        if (_topo->params[iter->second].static_.acc)
          _input.accModByParamThenNoteThenSample.push_back(MakeAccModEvent(iter->second, modFromHost));
      break;
    case CLAP_EVENT_PARAM_VALUE:
      valueFromHost = reinterpret_cast<clap_event_param_value const*>(header);
      if ((iter = _topo->paramTagToIndex.find(valueFromHost->param_id)) != _topo->paramTagToIndex.end())
      {
        normalized = FBCLAPToNormalized(_topo->params[iter->second].static_, valueFromHost->value);
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

  float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
  if (process->audio_inputs_count != 1)
    _input.audio = FBHostAudioBlock(zeroIn, process->frames_count);
  else
    _input.audio = FBHostAudioBlock(process->audio_inputs[0].data32, process->frames_count);
  _output.audio = FBHostAudioBlock(process->audio_outputs[0].data32, process->frames_count);

  _hostProcessor->ProcessHost(_input, _output);
  
  auto const& rvs = _output.returnedVoices;
  for (int rv = 0; rv < rvs.size(); rv++)
  {
    clap_event_note endEvent = MakeNoteEndEvent(rvs[rv], process->frames_count - 1);
    process->out_events->try_push(process->out_events, &(endEvent.header));
  }

  return CLAP_PROCESS_CONTINUE;
}