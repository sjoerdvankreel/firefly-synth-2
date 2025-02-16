#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <set>
#include <cassert>

FBProcStateContainer::
FBProcStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawProcState()),
_special(topo.static_.state.specialSelector(topo.static_, _rawState)),
_freeRawState(topo.static_.state.freeRawProcState)
{
  for (int p = 0; p < topo.audio.params.size(); p++)
    if (topo.static_.modules[topo.audio.params[p].topoIndices.module.index].voice)
      if (topo.audio.params[p].static_.acc)
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.addrSelectors.voiceAccProc(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));
      else
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.addrSelectors.voiceBlockProc(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));
    else
      if (topo.audio.params[p].static_.acc)
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.addrSelectors.globalAccProc(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));
      else
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.addrSelectors.globalBlockProc(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));

  for (int p = 0; p < Params().size(); p++)
    Params()[p].InitProcessing(topo.audio.params[p].static_.DefaultNormalizedByText());

#ifndef NDEBUG
  std::set<void*> uniquePtrs = {};
  for (int p = 0; p < _params.size(); p++)
    switch (_params[p].Type())
    {
    case FBProcParamType::VoiceAcc: uniquePtrs.insert(&_params[p].VoiceAcc()); break;
    case FBProcParamType::GlobalAcc: uniquePtrs.insert(&_params[p].GlobalAcc()); break;
    case FBProcParamType::VoiceBlock: uniquePtrs.insert(&_params[p].VoiceBlock()); break;
    case FBProcParamType::GlobalBlock: uniquePtrs.insert(&_params[p].GlobalBlock()); break;
    default: assert(false); break;
    }
  assert(uniquePtrs.size() == _params.size());
#endif
}

void 
FBProcStateContainer::InitProcessing(
  int index, float value)
{
  Params()[index].InitProcessing(value);
}

void
FBProcStateContainer::InitProcessing(
  int index, int voice, float value)
{
  Params()[index].InitProcessing(voice, value);
}

void
FBProcStateContainer::InitProcessing(
  FBScalarStateContainer const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    InitProcessing(p, *scalar.Params()[p]);
}

void 
FBProcStateContainer::InitProcessing(
  FBExchangeStateContainer const& exchange)
{
  for (int p = 0; p < Params().size(); p++)
    if (!Params()[p].IsVoice())
      InitProcessing(p, *exchange.Params()[p].Global());
    else
      for(int v = 0; v < FBMaxVoices; v++)
        if(exchange.Voices()[v].state == FBVoiceState::Active)
          InitProcessing(p, v, exchange.Params()[p].Voice()[v]);
}

void
FBProcStateContainer::SetSmoothingCoeffs(int sampleCount)
{
  if (_smoothingDurationSamples == sampleCount)
    return;
  _smoothingDurationSamples = sampleCount;
  for (int p = 0; p < Params().size(); p++)
    Params()[p].SetSmoothingCoeffs(sampleCount);
}