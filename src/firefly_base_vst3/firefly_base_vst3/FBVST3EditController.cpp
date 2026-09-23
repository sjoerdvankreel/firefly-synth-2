#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>
#include <firefly_base_vst3/FBVST3EditController.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <pluginterfaces/vst/ivstmidicontrollers.h>
#include <base/source/fstring.h>

#include <utility>
#include <algorithm>

using namespace juce;

static ParameterInfo
MakeMIDIParamInfo(int message, int controlChange)
{
  ParameterInfo result = {};
  result.stepCount = 0;
  result.unitId = 1;
  result.id = FBVST3MIDIParameterIDRangeBegin + message + controlChange;
  result.defaultNormalizedValue = 0.0;
  result.flags = ParameterInfo::kIsHidden;
  return result;
}

static ParameterInfo
MakePlugParamInfo(FBRuntimeParam const& param)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = 1;
  result.stepCount = std::max(0, param.static_.NonRealTime().ValueCount() - 1);
  result.defaultNormalizedValue = param.DefaultNormalizedByText();

  FBVST3CopyToString128(param.longName, result.title);
  FBVST3CopyToString128(param.shortName, result.shortTitle);
  FBVST3CopyToString128(param.static_.unit, result.units);

  result.flags = ParameterInfo::kNoFlags;
  if (param.static_.mode == FBParamMode::Accurate || param.static_.mode == FBParamMode::VoiceStart)
    result.flags = ParameterInfo::kCanAutomate;
  else
  {
    result.flags |= ParameterInfo::kIsHidden;
    result.flags |= ParameterInfo::kIsReadOnly;
    if (param.static_.NonRealTime().IsItems())
      result.flags |= ParameterInfo::kIsList;
  }
  return result;
}

FBVST3EditController::
~FBVST3EditController()
{
  FB_LOG_ENTRY_EXIT();
}

FBVST3EditController::
FBVST3EditController(std::unique_ptr<FBStaticTopo>&& topo) :
FBHostGUIContext(std::move(topo)),
_exchangeHandler(this)
{
  FB_LOG_ENTRY_EXIT();
}

void
FBVST3EditController::ResetView()
{
  FB_LOG_ENTRY_EXIT();
  _guiEditor = nullptr;
}

void
FBVST3EditController::DoEndAudioParamChange(int /*index*/)
{
  // see PerformAudioParamEdit
}

void
FBVST3EditController::DoBeginAudioParamChange(int /*index*/)
{
  // see PerformAudioParamEdit
}

void
FBVST3EditController::DoPerformAudioParamEdit(int index, double normalized)
{
  int tag = _topo->audio.params[index].tag;
  setParamNormalized(tag, normalized);

  // this is not ideal but it prevents automation getting ignored
  // once an automated parameter is user-modified on reaper
  beginEdit(tag);
  performEdit(tag, normalized);
  endEdit(tag);
}

void
FBVST3EditController::NotifyHostOfParamNameChanges()
{
  if (componentHandler)
    componentHandler->restartComponent(Vst::kParamTitlesChanged);
}

void 
FBVST3EditController::ClearAudioParamNameOverrides()
{
  for (int i = 0; i < parameters.getParameterCount(); i++)
    ClearAudioParamNameOverride(i);
}

void 
FBVST3EditController::ClearAudioParamNameOverride(int index)
{
  if(auto p = dynamic_cast<FBVST3Parameter*>(parameters.getParameterByIndex(index)))
    p->ClearNameOverride();
}

bool 
FBVST3EditController::GetAudioParamNameOverride(int index, std::string& name) const
{
  if(auto p = dynamic_cast<FBVST3Parameter*>(parameters.getParameterByIndex(index)))
    return p->GetNameOverride(name);
  return false;
}

void 
FBVST3EditController::SetAudioParamNameOverride(int index, std::string const& name)
{
  if(auto p = dynamic_cast<FBVST3Parameter*>(parameters.getParameterByIndex(index)))
    p->SetNameOverride(name);
}

double
FBVST3EditController::GetAudioParamNormalized(int index) const
{
  return parameters.getParameterByIndex(index)->getNormalized();
}

std::vector<FBHostContextMenuItem>
FBVST3EditController::MakeAudioParamContextMenu(int index)
{
  if (!_guiEditor)
    return {};
  return _guiEditor->MakeParamContextMenu(componentHandler, index);
}

void
FBVST3EditController::AudioParamContextMenuClicked(int paramIndex, int juceTag)
{
  if (_guiEditor)
    _guiEditor->ParamContextMenuClicked(componentHandler, paramIndex, juceTag);
}

tresult PLUGIN_API 
FBVST3EditController::setParamNormalized(ParamID tag, ParamValue value)
{
  tresult result = EditControllerEx1::setParamNormalized(tag, value);
  if(result != kResultTrue || tag >= FBVST3MIDIParameterIDRangeBegin)
    return result;
  if (_guiEditor != nullptr)
    _guiEditor->SetAudioParamNormalizedFromHost(_topo->audio.paramTagToIndex[tag], value);
  return kResultTrue;
}

tresult PLUGIN_API 
FBVST3EditController::notify(IMessage* message)
{
  return FBWithLogException([this, message]()
  {
    if (_exchangeHandler.onMessage(message))
      return static_cast<tresult>(kResultTrue);
    return EditControllerEx1::notify(message);
  });
}

IPlugView* PLUGIN_API
FBVST3EditController::createView(FIDString name)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, name]()
  {
    if (ConstString(name) != ViewType::kEditor)
      return static_cast<IPlugView*>(nullptr);
    if (_guiEditor == nullptr)
      _guiEditor = new FBVST3GUIEditor(this);
    return static_cast<IPlugView*>(_guiEditor);
  });
}

tresult PLUGIN_API
FBVST3EditController::getState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, state]()
  {
    std::string json = _topo->SaveGUIStateToString(*this);
    if (!FBVST3SaveIBStream(state, json))
      return kResultFalse;
    return kResultOk;
  });
}

tresult PLUGIN_API 
FBVST3EditController::setState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, state]()
  {
    std::string json;
    if (!FBVST3LoadIBStream(state, json))
      return kResultFalse;
    _topo->LoadGUIStateFromStringWithDryRun(json, *this);
    OnPatchNameChanged();
    OnInstanceNameChanged();    
    if(_guiEditor != nullptr)
      for (int i = 0; i < _guiState->Params().size(); i++)
        _guiEditor->SetGUIParamNormalizedFromHost(i, GetGUIParamNormalized(i));      
    MarkPatchAsSessionState();
    NotifyHostOfParamNameChanges();
    return kResultTrue;
  });
}

tresult PLUGIN_API
FBVST3EditController::setComponentState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, state]()
  {
    std::string json;
    if (!FBVST3LoadIBStream(state, json))
      return kResultFalse;
    FBScalarStateContainer edit(*_topo);
    if (!_topo->LoadEditStateFromString(json, edit, false))
      return kResultFalse;
    for (int i = 0; i < edit.Params().size(); i++)
      setParamNormalized(_topo->audio.params[i].tag, *edit.Params()[i]);
    MarkPatchAsSessionState();
    NotifyHostOfParamNameChanges();
    return kResultOk;
  });
}

tresult PLUGIN_API
FBVST3EditController::initialize(FUnknown* context)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, context]()
  {
    if (EditController::initialize(context) != kResultTrue)
      return kResultFalse;

    // create top root unit with kProgramId as id for the programList
    addUnit(new Unit(STR("Root"), kRootUnitId, kNoParentUnitId));
    addUnit(new Unit(STR("Main"), 1, kRootUnitId, 1));

    // create the program list: here kNumProgs entries
    auto* prgList = new ProgramList(STR("Bank"), 1, 1);
    addProgramList(prgList);
    for (int32 i = 0; i < 10; i++)
    {
      std::u16string title = STR("Prog ");
      title += (char16_t)'a' + i;
      prgList->addProgram(title.data());
    }

    //---Program Change parameter---
    Parameter* prgParam = prgList->getParameter();

    // by default this program change parameter if automatable we can overwrite this:
    prgParam->getInfo().flags &= ~ParameterInfo::kCanAutomate;

    parameters.addParameter(prgParam);

    for (int m = 0; m < _topo->modules.size(); m++)
    {
      for (int p = 0; p < _topo->modules[m].params.size(); p++)
      {
        FB_ASSERT(_topo->modules[m].params[p].tag < FBVST3ReservedParameterIDRangeBegin);
        auto const& topo = _topo->modules[m].params[p];
        auto info = MakePlugParamInfo(topo);
        parameters.addParameter(new FBVST3Parameter(this, &topo, info));
      }
    }

    for (int i = 0; i < FBMIDIEvent::CCMessageCount; i++)
      parameters.addParameter(new Parameter(MakeMIDIParamInfo(FBMIDIEvent::CCMessageId, i)));
    parameters.addParameter(new Parameter(MakeMIDIParamInfo(FBMIDIEvent::CPMessageId, 0)));
    parameters.addParameter(new Parameter(MakeMIDIParamInfo(FBMIDIEvent::PBMessageId, 0)));

    return kResultTrue;
  });
}

tresult PLUGIN_API FBVST3EditController::getUnitByBus(
  MediaType type, BusDirection dir, int32 busIndex,
  int32 channel, UnitID& unitId)
{
  if (type == kEvent && dir == kInput && busIndex == 0 && channel == 0)
  {
    unitId = 1;
    return kResultTrue;
  }
  return kResultFalse;
}

tresult PLUGIN_API 
FBVST3EditController::getMidiControllerAssignment(
  int32 bus, int16, CtrlNumber number, ParamID& id)
{
  return FBWithLogException([bus, number, &id]() {
    if (bus != 0)
      return kResultFalse;
    if (!(0 <= number && number < kCountCtrlNumber))
      return kResultFalse;
    id = FBVST3MIDIParameterIDRangeBegin + number;
    return kResultTrue;
  });
}

void PLUGIN_API
FBVST3EditController::onDataExchangeBlocksReceived(
  DataExchangeUserContextID /*id*/, uint32 numBlocks, DataExchangeBlock* blocks, TBool /*bg*/)
{
  FBWithLogException([this, numBlocks, blocks]()
  {
    if (numBlocks == 0)
      return;
    memcpy(_exchangeFromDSPState->Raw(), blocks[numBlocks - 1].data, _topo->static_->exchangeStateSize);
    if (_guiEditor != nullptr)
      _guiEditor->UpdateExchangeState();
  });
}