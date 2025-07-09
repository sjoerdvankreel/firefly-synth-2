#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

#include <array>

struct FBModuleProcExchangeStateBase
{
  bool active = {};
  virtual ~FBModuleProcExchangeStateBase() = 0 {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleProcExchangeStateBase);

  virtual bool ShouldGraph(int graphIndex) const = 0;
  virtual int LengthSamples(int graphIndex) const = 0;
  virtual float PositionNormalized(int graphIndex) const = 0;
};

template <int N>
struct FBModuleProcExchangeState final:
public FBModuleProcExchangeStateBase
{
  std::array<int, N> lengthSamples = {};
  std::array<int, N> positionSamples = {};
  ~FBModuleProcExchangeState() = default;
  FB_COPY_MOVE_DEFCTOR(FBModuleProcExchangeState);

  int LengthSamples(int graphIndex) const override
  { return lengthSamples[graphIndex]; }
  bool ShouldGraph(int graphIndex) const override 
  { return active && positionSamples[graphIndex] < lengthSamples[graphIndex]; }
  float PositionNormalized(int graphIndex) const override 
  { return positionSamples[graphIndex] / static_cast<float>(lengthSamples[graphIndex]); }
};

class FBModuleExchangeState final
{
  friend class FBHostProcessor;
  friend class FBExchangeStateContainer;

  FBModuleProcExchangeStateBase* _global;
  std::array<FBModuleProcExchangeStateBase*, FBMaxVoices> _voice;

  FBModuleProcExchangeStateBase* Global();
  std::array<FBModuleProcExchangeStateBase*, FBMaxVoices>& Voice();

public:
  bool IsGlobal() const { return _global != nullptr; }
  FBModuleProcExchangeStateBase const* Global() const;
  std::array<FBModuleProcExchangeStateBase*, FBMaxVoices> const& Voice() const;

  FB_NOCOPY_MOVE_DEFCTOR(FBModuleExchangeState);
  explicit FBModuleExchangeState(FBModuleProcExchangeStateBase* global);
  explicit FBModuleExchangeState(std::array<FBModuleProcExchangeStateBase*, FBMaxVoices> const& voice);
};

inline FBModuleExchangeState::
FBModuleExchangeState(FBModuleProcExchangeStateBase* global) :
_global(global),
_voice() {}

inline FBModuleExchangeState::
FBModuleExchangeState(std::array<FBModuleProcExchangeStateBase*, FBMaxVoices> const& voice) :
_global(nullptr),
_voice(voice) {}

inline FBModuleProcExchangeStateBase*
FBModuleExchangeState::Global()
{
  FB_ASSERT(IsGlobal());
  return _global;
}

inline FBModuleProcExchangeStateBase const*
FBModuleExchangeState::Global() const
{
  FB_ASSERT(IsGlobal());
  return _global;
}

inline std::array<FBModuleProcExchangeStateBase*, FBMaxVoices>&
FBModuleExchangeState::Voice()
{
  FB_ASSERT(!IsGlobal());
  return _voice;
}

inline std::array<FBModuleProcExchangeStateBase*, FBMaxVoices> const&
FBModuleExchangeState::Voice() const
{
  FB_ASSERT(!IsGlobal());
  return _voice;
}