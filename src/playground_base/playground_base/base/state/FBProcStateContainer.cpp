#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>

#include <set>
#include <cassert>

FBProcStateContainer::
FBProcStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawProcState()),
_special(topo.static_.state.specialSelector(topo.static_, _rawState)),
_freeRawState(topo.static_.state.freeRawProcState)
{
  for (int p = 0; p < topo.params.size(); p++)
    if (topo.static_.modules[topo.params[p].topoIndices.module.index].voice)
      if (topo.params[p].static_.acc)
        _params.push_back(FBProcParamState(topo.params[p].static_.voiceAccAddr(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));
      else
        _params.push_back(FBProcParamState(topo.params[p].static_.voiceBlockAddr(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));
    else
      if (topo.params[p].static_.acc)
        _params.push_back(FBProcParamState(topo.params[p].static_.globalAccAddr(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));
      else
        _params.push_back(FBProcParamState(topo.params[p].static_.globalBlockAddr(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));

  for (int p = 0; p < Params().size(); p++)
    Params()[p].InitProcessing(topo.params[p].static_.DefaultNormalizedByText());

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
  FBScalarStateContainer const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    InitProcessing(p, *scalar.Params()[p]);
}

void 
FBProcStateContainer::InitProcessing(
  FBExchangeStateContainer const& exchange, int voice)
{
  for (int p = 0; p < Params().size(); p++)
    if (Params()[p].IsVoice())
      InitProcessing(p, exchange.Params()[p].Voice()[voice]);
    else
      InitProcessing(p, *exchange.Params()[p].Global());    
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