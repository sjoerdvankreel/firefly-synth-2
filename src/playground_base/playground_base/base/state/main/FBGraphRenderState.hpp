#pragma once

#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <memory>

struct FBPlugInputBlock;
struct FBModuleProcState;

class FBPlugGUI;
class FBVoiceManager;
class FBProcStateContainer;
class FBScalarStateContainer;

class FBGraphRenderState final
{
  friend class FBModuleGraphComponent;

  FBFixedFloatAudioBlock _audio = {};
  std::vector<FBNoteEvent> _notes = {};

  FBPlugGUI const* const _plugGUI;
  std::unique_ptr<FBPlugInputBlock> _input;
  std::unique_ptr<FBModuleProcState> _moduleState;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBScalarStateContainer> _scalarState;
  std::unique_ptr<FBVoiceManager> _primaryVoiceManager;
  std::unique_ptr<FBVoiceManager> _exchangeVoiceManager;

public:
  ~FBGraphRenderState();
  FBGraphRenderState(FBPlugGUI const* plugGUI);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGraphRenderState);

  void PrepareForRenderPrimary();
  void PrepareForRenderExchange();
  void PrepareForRenderPrimaryVoice();
  void PrepareForRenderExchangeVoice(int voice);

  FBModuleProcState* ModuleProcState();
  FBScalarStateContainer const* ScalarContainer() const;
  FBExchangeStateContainer const* ExchangeContainer() const;

  void PrimaryParamChanged(int index, float normalized);
  bool GlobalModuleExchangeStateEqualsPrimary(int moduleIndex, int moduleSlot) const;
  bool VoiceModuleExchangeStateEqualsPrimary(int voice, int moduleIndex, int moduleSlot) const;
};