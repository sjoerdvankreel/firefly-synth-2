#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>

#include <set>
#include <cassert>

FBExchangeStateContainer::
FBExchangeStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawExchangeState()),
_freeRawState(topo.static_.state.freeRawExchangeState)
{
  _voiceState = topo.static_.state.voiceStateExchangeAddr(_rawState);

  for (int m = 0; m < topo.modules.size(); m++)
  {
    auto const& indices = topo.modules[m].topoIndices;
    auto const& static_ = topo.static_.modules[indices.index];
    if(!static_.voice)
      _active.push_back(FBExchangeActiveState(
        static_.addrSelectors.globalExchangeActive(
          indices.slot, _rawState)));
    else
    {
      std::array<bool*, FBMaxVoices> voiceActive = {};
      for (int v = 0; v < FBMaxVoices; v++)
        voiceActive[v] = static_.addrSelectors.voiceExchangeActive(
          v, indices.slot, _rawState);
      _active.push_back(FBExchangeActiveState(voiceActive));
    }
  }

  for (int p = 0; p < topo.params.size(); p++)
    if (topo.static_.modules[topo.params[p].topoIndices.module.index].voice)
      _params.push_back(FBExchangeParamState(
        topo.params[p].static_.addrSelectors.voiceExchange(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));
    else
      _params.push_back(FBExchangeParamState(
        topo.params[p].static_.addrSelectors.globalExchange(
          topo.params[p].topoIndices.module.slot,
          topo.params[p].topoIndices.param.slot, _rawState)));

#ifndef NDEBUG
  std::set<void const*> uniquePtrs = {};
  for (int p = 0; p < _params.size(); p++)
    if (_params[p].IsGlobal())
      uniquePtrs.insert(_params[p].Global());
    else
      uniquePtrs.insert(&_params[p].Voice());
  assert(uniquePtrs.size() == _params.size());
#endif
}

FBExchangeParamActiveState 
FBExchangeStateContainer::GetParamActiveState(int paramIndex) const
{
  FBExchangeParamActiveState result = {};
  result.active = false;
  result.minValue = 1.0f;
  result.maxValue = 0.0f;

  float exchangeValue = 0.0f;
  auto const& param = Params()[paramIndex];
  auto const& active = Active()[paramIndex];

  if (param.IsGlobal())
    if (*active.Global())
    {
      result.active = true;
      result.minValue = std::min(result.minValue, *param.Global());
      result.maxValue = std::max(result.maxValue, *param.Global());
    }
  if (!param.IsGlobal())
    for (int v = 0; v < FBMaxVoices; v++)
      if (*active.Voice()[v])
      {
        result.active = true;
        result.minValue = std::min(result.minValue, param.Voice()[v]);
        result.maxValue = std::max(result.maxValue, param.Voice()[v]);
      }
 
  return result;
}