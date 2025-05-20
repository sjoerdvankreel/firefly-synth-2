#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci_base/FFOsciTopoBase.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

void
FFOsciProcessorBase::BeginVoice(FBModuleProcState& state, int uniCount)
{
  _uniCount = uniCount;
  _key = static_cast<float>(state.voice->event.note.key);
  for (int u = 0; u < _uniCount; u++)
  {
    if (_uniCount == 1)
    {
      _uniPosMHalfToHalf.Set(u, 0.0f);
      _uniPosAbsHalfToHalf.Set(u, 0.0f);
    }
    else
    {
      _uniPosMHalfToHalf.Set(u, u / (_uniCount - 1.0f) - 0.5f);
      _uniPosAbsHalfToHalf.Set(u, std::fabs(_uniPosMHalfToHalf.Get(u)));
    }
  }
}

void 
FFOsciProcessorBase::ProcessGainSpreadBlend(
  FBSArray2<float, FBFixedBlockSamples, 2>& output)
{
  for (int u = 0; u < _uniCount; u++)
  {
    float uniPosMHalfToHalf = _uniPosMHalfToHalf.Get(u);
    float uniPosAbsHalfToHalf = _uniPosAbsHalfToHalf.Get(u);
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto uniPanning = 0.5f + uniPosMHalfToHalf * _uniSpreadPlain.Load(s);
      auto uniBlend = 1.0f - (uniPosAbsHalfToHalf * 2.0f * (1.0f - _uniBlendPlain.Load(s)));
      auto uniMono = _uniOutput[u].Load(s) * _gainPlain.Load(s) * uniBlend;
      output[0].Add(s, (1.0f - uniPanning) * uniMono);
      output[1].Add(s, uniPanning * uniMono);
    }
  }
  output.NaNCheck();
}