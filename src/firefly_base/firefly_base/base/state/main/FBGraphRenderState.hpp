#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>

#include <memory>

struct FBPlugInputBlock;
struct FBModuleProcState;

class FBPlugGUI;
class FBVoiceManager;
class FBProcStateContainer;
class FBExchangeStateContainer;

class FBGraphRenderState final
{
  friend class FBModuleGraphDisplayComponent;

  std::vector<FBNoteEvent> _noteEvents = {};
  FBSArray2<float, FBFixedBlockSamples, 2> _audio = {};

  FBPlugGUI const* const _plugGUI;
  std::unique_ptr<FBPlugInputBlock> _input;
  std::unique_ptr<FBModuleProcState> _moduleState;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBVoiceManager> _primaryVoiceManager;
  std::unique_ptr<FBVoiceManager> _exchangeVoiceManager;

  double GetAudioParamNormalized(
    FBParamTopoIndices const& indices,
    bool exchange, int exchangeVoice) const;

public:
  ~FBGraphRenderState();
  FBGraphRenderState(FBPlugGUI const* plugGUI);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGraphRenderState);

  void PrepareForRenderPrimaryVoice();
  void PrepareForRenderExchangeVoice(int voice);
  void PrepareForRenderExchange(float lastNote);
  void PrepareForRenderPrimary(float sampleRate, float bpm);

  FBPlugGUI const* PlugGUI() const;
  FBModuleProcState* ModuleProcState();
  FBModuleProcState const* ModuleProcState() const;
  FBExchangeStateContainer const* ExchangeContainer() const;
  bool GlobalModuleExchangeStateEqualsPrimary(int moduleIndex, int moduleSlot) const;
  bool VoiceModuleExchangeStateEqualsPrimary(int voice, int moduleIndex, int moduleSlot) const;

  bool GUIParamBool(FBParamTopoIndices const& indices) const;
  float GUIParamLinear(FBParamTopoIndices const& indices) const;
  int GUIParamLinearTimeSamples(FBParamTopoIndices const& indices, float sampleRate) const;
  int GUIParamBarsSamples(FBParamTopoIndices const& indices, float sampleRate, float bpm) const;

  bool AudioParamBool(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice) const;
  int AudioParamDiscrete(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice) const;
  float AudioParamLinear(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice) const;
  template <class T> T AudioParamList(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice) const;
  int AudioParamLinearFreqSamples(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice, float sampleRate) const;
  int AudioParamLinearTimeSamples(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice, float sampleRate) const;
  int AudioParamBarsSamples(FBParamTopoIndices const& indices, bool exchange, int exchangeVoice, float sampleRate, float bpm) const;
};

template <class T> T
FBGraphRenderState::AudioParamList(
  FBParamTopoIndices const& indices, bool exchange, int exchangeVoice) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(indices, exchange, exchangeVoice);
  return static_cast<T>(param->static_.List().NormalizedToPlainFast(static_cast<float>(normalized)));
}