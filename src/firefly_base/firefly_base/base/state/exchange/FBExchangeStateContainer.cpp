#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <set>

FBExchangeStateContainer::
FBExchangeStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_->allocRawExchangeState()),
_freeRawState(topo.static_->freeRawExchangeState),
_host(topo.static_->hostExchangeAddr(_rawState)),
_voices(topo.static_->voicesExchangeAddr(_rawState))
{
  for (int m = 0; m < topo.modules.size(); m++)
  {
    auto const& indices = topo.modules[m].topoIndices;
    auto const& static_ = topo.static_->modules[indices.index];
    if (!static_.voice && static_.globalModuleExchangeAddr)
    {
      _modules.push_back(std::make_unique<FBModuleExchangeState>(
        static_.globalModuleExchangeAddr(indices.slot, _rawState)));
    }
    else if(static_.voice && static_.voiceModuleExchangeAddr)
    {
      std::array<FBModuleProcExchangeStateBase*, FBMaxVoices> moduleExchange = {};
      for (int v = 0; v < FBMaxVoices; v++)
        moduleExchange[v] = static_.voiceModuleExchangeAddr(v, indices.slot, _rawState);
      _modules.push_back(std::make_unique<FBModuleExchangeState>(moduleExchange));
    }
    else
    {
      _modules.push_back(std::unique_ptr<FBModuleExchangeState>());
    }
  }

  for (int p = 0; p < topo.audio.params.size(); p++)
    if (topo.static_->modules[topo.audio.params[p].topoIndices.module.index].voice)
      _params.push_back(FBParamExchangeState(
        topo.audio.params[p].static_.voiceExchangeAddr(
          topo.audio.params[p].topoIndices.module.slot,
          topo.audio.params[p].topoIndices.param.slot, _rawState)));
    else
      _params.push_back(FBParamExchangeState(
        topo.audio.params[p].static_.globalExchangeAddr(
          topo.audio.params[p].topoIndices.module.slot,
          topo.audio.params[p].topoIndices.param.slot, _rawState)));

#ifndef NDEBUG
  std::set<void const*> uniquePtrs = {};
  for (int p = 0; p < _params.size(); p++)
    if (_params[p].IsGlobal())
      uniquePtrs.insert(_params[p].Global());
    else
      uniquePtrs.insert(&_params[p].Voice());
  FB_ASSERT(uniquePtrs.size() == _params.size());
#endif
}

FBParamActiveExchangeState
FBExchangeStateContainer::GetParamActiveState(FBRuntimeParam const* param) const
{
  FBParamActiveExchangeState result = {};
  result.active = false;
  result.minValue = 1.0f;
  result.maxValue = 0.0f;

  auto const& paramExchange = Params()[param->runtimeParamIndex];
  auto const& moduleExchange = Modules()[param->runtimeModuleIndex];

  if (moduleExchange.get() == nullptr)
    return result;

  if (paramExchange.IsGlobal())
    if (moduleExchange->Global()->boolIsActive != 0)
    {
      result.active = true;
      result.minValue = std::min(result.minValue, *paramExchange.Global());
      result.maxValue = std::max(result.maxValue, *paramExchange.Global());
    }
  if (!paramExchange.IsGlobal())
    for (int v = 0; v < FBMaxVoices; v++)
      if (moduleExchange->Voice()[v]->boolIsActive != 0)
      {
        result.active = true;
        result.minValue = std::min(result.minValue, paramExchange.Voice()[v]);
        result.maxValue = std::max(result.maxValue, paramExchange.Voice()[v]);
      }
 
  return result;
}