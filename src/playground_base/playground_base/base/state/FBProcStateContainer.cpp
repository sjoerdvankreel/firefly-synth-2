#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <set>
#include <cassert>

FBProcStateContainer::
FBProcStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawProcState()),
_special(topo.static_.state.specialSelector(topo.static_, _rawState)),
_freeRawState(topo.static_.state.freeRawProcState)
{
  for (int p = 0; p < topo.params.size(); p++)
    if (topo.static_.modules[topo.params[p].topoIndices.staticModuleIndex].voice)
      if (topo.params[p].static_.acc)
        _params.push_back(FBProcParamState(topo.params[p].static_.voiceAccAddr(
          topo.params[p].topoIndices.staticModuleSlot, 
          topo.params[p].topoIndices.staticParamSlot, _rawState)));
      else
        _params.push_back(FBProcParamState(topo.params[p].static_.voiceBlockAddr(
          topo.params[p].topoIndices.staticModuleSlot, 
          topo.params[p].topoIndices.staticParamSlot, _rawState)));
    else
      if (topo.params[p].static_.acc)
        _params.push_back(FBProcParamState(topo.params[p].static_.globalAccAddr(
          topo.params[p].topoIndices.staticModuleSlot, 
          topo.params[p].topoIndices.staticParamSlot, _rawState)));
      else
        _params.push_back(FBProcParamState(topo.params[p].static_.globalBlockAddr(
          topo.params[p].topoIndices.staticModuleSlot, 
          topo.params[p].topoIndices.staticParamSlot, _rawState)));

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
FBProcStateContainer::InitProcessing(int index, float value)
{
  Params()[index].InitProcessing(value);
}

void
FBProcStateContainer::InitProcessing(FBScalarStateContainer const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    InitProcessing(p, *scalar.Params()[p]);
}

void
FBProcStateContainer::SetSmoothingCoeffs(float sampleRate, float durationSecs)
{
  if (_smoothingDurationSecs == durationSecs)
    return;
  _smoothingDurationSecs = durationSecs;
  for (int p = 0; p < Params().size(); p++)
    Params()[p].SetSmoothingCoeffs(sampleRate, durationSecs);
}