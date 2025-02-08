#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>
#include <playground_base/base/state/exchange/FBModuleProcExchangeState.hpp>

#include <set>
#include <cassert>

FBExchangeStateContainer::
FBExchangeStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_.state.allocRawExchangeState()),
_freeRawState(topo.static_.state.freeRawExchangeState)
{
  _voices = topo.static_.state.voiceStateExchangeAddr(_rawState);

  for (int m = 0; m < topo.modules.size(); m++)
  {
    auto const& indices = topo.modules[m].topoIndices;
    auto const& static_ = topo.static_.modules[indices.index];
    if(!static_.voice)
      _modules.push_back(FBModuleExchangeState(
        static_.addrSelectors.globalExchangeActive(
          indices.slot, _rawState)));
    else
    {
      std::array<bool*, FBMaxVoices> voiceActive = {};
      for (int v = 0; v < FBMaxVoices; v++)
        voiceActive[v] = static_.addrSelectors.voiceExchangeActive(
          v, indices.slot, _rawState);
      _modules.push_back(FBModuleExchangeState(voiceActive));
    }
  }

  for (int p = 0; p < topo.params.size(); p++)
    if (topo.static_.modules[topo.params[p].topoIndices.module.index].voice)
      _params.push_back(FBParamExchangeState(
        topo.params[p].static_.addrSelectors.voiceExchange(
          topo.params[p].topoIndices.module.slot, 
          topo.params[p].topoIndices.param.slot, _rawState)));
    else
      _params.push_back(FBParamExchangeState(
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

FBParamActiveExchangeState
FBExchangeStateContainer::GetParamActiveState(FBRuntimeParam const* param) const
{
  FBParamActiveExchangeState result = {};
  result.active = false;
  result.minValue = 1.0f;
  result.maxValue = 0.0f;

  float exchangeValue = 0.0f;
  auto const& paramExchange = Params()[param->runtimeParamIndex];
  auto const& activeExchange = Active()[param->runtimeModuleIndex];

  if (paramExchange.IsGlobal())
    if (*activeExchange.Global())
    {
      result.active = true;
      result.minValue = std::min(result.minValue, *paramExchange.Global());
      result.maxValue = std::max(result.maxValue, *paramExchange.Global());
    }
  if (!paramExchange.IsGlobal())
    for (int v = 0; v < FBMaxVoices; v++)
      if (*activeExchange.Voice()[v])
      {
        result.active = true;
        result.minValue = std::min(result.minValue, paramExchange.Voice()[v]);
        result.maxValue = std::max(result.maxValue, paramExchange.Voice()[v]);
      }
 
  return result;
}