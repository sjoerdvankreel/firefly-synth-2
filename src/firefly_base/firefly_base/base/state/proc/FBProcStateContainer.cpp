#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <set>

FBProcStateContainer::
FBProcStateContainer(FBRuntimeTopo const& topo):
_rawState(topo.static_->allocRawProcState()),
_freeRawState(topo.static_->freeRawProcState),
_special(topo.static_->specialSelector(*topo.static_, _rawState))
{
  for (int p = 0; p < topo.audio.params.size(); p++)
    if (topo.static_->modules[topo.audio.params[p].topoIndices.module.index].voice)
      if (topo.audio.params[p].static_.acc)
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.voiceAccProcAddr(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));
      else
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.voiceBlockProcAddr(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));
    else
      if (topo.audio.params[p].static_.acc)
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.globalAccProcAddr(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));
      else
        _params.push_back(FBProcParamState(
          topo.audio.params[p].static_.globalBlockProcAddr(
            topo.audio.params[p].topoIndices.module.slot,
            topo.audio.params[p].topoIndices.param.slot, _rawState)));

  for (int p = 0; p < Params().size(); p++)
    Params()[p].InitProcessing(static_cast<float>(topo.audio.params[p].DefaultNormalizedByText()));

#ifndef NDEBUG
  std::set<void*> uniquePtrs = {};
  for (int p = 0; p < _params.size(); p++)
    switch (_params[p].Type())
    {
    case FBProcParamType::VoiceAcc: uniquePtrs.insert(&_params[p].VoiceAcc()); break;
    case FBProcParamType::GlobalAcc: uniquePtrs.insert(&_params[p].GlobalAcc()); break;
    case FBProcParamType::VoiceBlock: uniquePtrs.insert(&_params[p].VoiceBlock()); break;
    case FBProcParamType::GlobalBlock: uniquePtrs.insert(&_params[p].GlobalBlock()); break;
    default: FB_ASSERT(false); break;
    }
  FB_ASSERT(uniquePtrs.size() == _params.size());
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
    InitProcessing(p, static_cast<float>(*scalar.Params()[p]));
}

void 
FBProcStateContainer::InitProcessing(
  FBHostGUIContext const* context)
{
  for (int p = 0; p < Params().size(); p++)
    InitProcessing(p, static_cast<float>(context->GetAudioParamNormalized(p)));
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
  for (int m = 0; m < MIDIParams().size(); m++)
    MIDIParams()[m].Global().SetSmoothingCoeffs(sampleCount);
}