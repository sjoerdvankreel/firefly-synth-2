#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

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
  std::unique_ptr<FBVoiceManager> _voiceManager;

public:
  ~FBGraphRenderState();
  FBGraphRenderState(FBPlugGUI const* plugGUI);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGraphRenderState);

  void PrepareForRender(bool primary, int voice);
  void PrimaryParamChanged(int index, float normalized);
  FBExchangeStateContainer const* ExchangeContainer() const;
  
  FBModuleProcState& ModuleState() 
  { return *_moduleState; }
  template <class TExchangeState>
  TExchangeState const* ExchangeState() 
  { return static_cast<TExchangeState const*>(ExchangeContainer()->Raw()); }
};