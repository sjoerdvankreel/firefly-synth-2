#include <firefly_base_vst3/FBVST3Plugin.hpp>
#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>

#include <pluginterfaces/vst/ivstmidicontrollers.h>
#include <base/source/fstring.h>

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
MakeMIDIParamInfo(int message, int controlChange)
{
  ParameterInfo result = {};
  result.stepCount = 0;
  result.unitId = kRootUnitId;
  result.id = FBVST3MIDIParameterMappingBegin + message + controlChange;
  result.defaultNormalizedValue = 0.0;
  result.flags = ParameterInfo::kIsHidden | ParameterInfo::kIsReadOnly;
  return result;
}

static ParameterInfo
MakePlugParamInfo(FBRuntimeParam const& param, int unitId)
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

tresult PLUGIN_API
FBVST3Plugin::setComponentState(IBStream* state)
{
  (void)state;
  FB_LOG_ENTRY_EXIT();
  return kResultOk;
}

tresult PLUGIN_API
FBVST3Plugin::getEditorState(IBStream* state)
{
  (void)state;
  FB_LOG_ENTRY_EXIT();
  return kResultOk;
}

tresult PLUGIN_API
FBVST3Plugin::setEditorState(IBStream* state)
{
  (void)state;
  FB_LOG_ENTRY_EXIT();
  return kResultOk;
}


IPlugView* PLUGIN_API
FBVST3Plugin::createView(FIDString name)
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
FBVST3Plugin::getMidiControllerAssignment(
  int32 bus, int16, CtrlNumber number, ParamID& id)
{
  return FBWithLogException([bus, number, &id]() {
    if (bus != 0)
      return kResultFalse;
    if (!(0 <= number && number < kCountCtrlNumber))
      return kResultFalse;
    id = FBVST3MIDIParameterMappingBegin + number;
    return kResultTrue;
  });
}

tresult PLUGIN_API
FBVST3Plugin::setParamNormalized(ParamID tag, ParamValue value)
{
  tresult result = SingleComponentEffect::setParamNormalized(tag, value);
  if (result != kResultTrue || tag >= FBVST3MIDIParameterMappingBegin)
    return result;
  if (_guiEditor != nullptr)
    _guiEditor->SetAudioParamNormalizedFromHost(Topo()->audio.paramTagToIndex.at(tag), value);
  return kResultTrue;
}

void
FBVST3Plugin::InitializeEditController()
{
  FB_LOG_ENTRY_EXIT();
  FBWithLogException([this]()
  {
    int unitId = 1;
    for (int m = 0; m < Topo()->modules.size(); m++)
    {
      addUnit(new Unit(MakeUnitInfo(Topo()->modules[m], unitId)));
      for (int p = 0; p < Topo()->modules[m].params.size(); p++)
      {
        FB_ASSERT(Topo()->modules[m].params[p].tag < FBVST3MIDIParameterMappingBegin);
        auto const& topo = Topo()->modules[m].params[p];
        auto info = MakePlugParamInfo(topo, unitId);
        parameters.addParameter(new FBVST3Parameter(this, &topo, info));
      }
      unitId++;
    }

    for (int i = 0; i < FBMIDIEvent::CCMessageCount; i++)
      parameters.addParameter(new Parameter(MakeMIDIParamInfo(FBMIDIEvent::CCMessageId, i)));
    parameters.addParameter(new Parameter(MakeMIDIParamInfo(FBMIDIEvent::CPMessageId, 0)));
    parameters.addParameter(new Parameter(MakeMIDIParamInfo(FBMIDIEvent::PBMessageId, 0)));
  });
}