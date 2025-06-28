#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>
#include <firefly_base_vst3/FBVST3EditController.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <base/source/fstring.h>
#include <utility>
#include <algorithm>

using namespace juce;

static UnitInfo
MakeUnitInfo(FBRuntimeModule const& module, int id)
{
  UnitInfo result;
  result.id = id;
  result.parentUnitId = kRootUnitId;
  result.programListId = kNoProgramListId;
  FBVST3CopyToString128(module.name, result.name);
  return result;
}

static ParameterInfo
MakeParamInfo(FBRuntimeParam const& param, int unitId)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = unitId;
  result.stepCount = std::max(0, param.static_.NonRealTime().ValueCount() - 1);
  result.defaultNormalizedValue = param.DefaultNormalizedByText();

  FBVST3CopyToString128(param.longName, result.title);
  FBVST3CopyToString128(param.shortName, result.shortTitle);
  FBVST3CopyToString128(param.static_.unit, result.units);

  result.flags = ParameterInfo::kNoFlags;
  if (param.static_.AutomationTiming() != FBAutomationTiming::Never)
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
FBVST3EditController(FBStaticTopo const& topo) :
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_guiState(std::make_unique<FBGUIStateContainer>(topo)),
_exchangeState(std::make_unique<FBExchangeStateContainer>(topo)),
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

double
FBVST3EditController::GetAudioParamNormalized(int index) const
{
  return parameters.getParameterByIndex(index)->getNormalized();
}

double
FBVST3EditController::GetGUIParamNormalized(int index) const
{
  return *_guiState->Params()[index];
}

void 
FBVST3EditController::SetGUIParamNormalized(int index, double normalized)
{
  *_guiState->Params()[index] = normalized;
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
  if(EditControllerEx1::setParamNormalized(tag, value) != kResultTrue)
    return kResultFalse;
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
    std::string json = _topo->SaveGUIStateToString(*_guiState);
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
    UndoState().BeginAction("Load State");
    _topo->LoadGUIStateFromStringWithDryRun(json, *_guiState);
    UndoState().EndAction();
    UndoState().Clear();
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
    if (!_topo->LoadEditStateFromString(json, edit))
      return kResultFalse;
    UndoState().BeginAction("Load State");
    for (int i = 0; i < edit.Params().size(); i++)
      setParamNormalized(_topo->audio.params[i].tag, *edit.Params()[i]);
    UndoState().EndAction();
    UndoState().Clear();
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

    int unitId = 1;
    for (int m = 0; m < _topo->modules.size(); m++)
    {
      addUnit(new Unit(MakeUnitInfo(_topo->modules[m], unitId)));
      for (int p = 0; p < _topo->modules[m].params.size(); p++)
      {
        auto const& topo = _topo->modules[m].params[p];
        auto info = MakeParamInfo(topo, unitId);
        parameters.addParameter(new FBVST3Parameter(&topo.static_, info));
      }
      unitId++;
    }
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
    memcpy(_exchangeState->Raw(), blocks[numBlocks - 1].data, _topo->static_.exchangeStateSize);
    if (_guiEditor != nullptr)
      _guiEditor->UpdateExchangeState();
  });
}