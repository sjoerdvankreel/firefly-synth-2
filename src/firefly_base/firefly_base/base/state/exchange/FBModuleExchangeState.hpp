#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

#include <array>

// It doesn't feel so great to push stuff with virtuals over the wire.
// It seems to work, and even cross-process (Bitwig), and we're guaranteed
// to be always the exact same binary on both ends, etc, but still. I don't like it.
// NOTE TO SELF: at blit-time, it's always most-derived and always by-value,
// so, we're probably good.
struct alignas(FBSIMDAlign) FBModuleProcExchangeStateBase
{
  int boolIsActive = 0; // i dont dare to put not aligned stuff in here anymore because of ARM mac
  virtual ~FBModuleProcExchangeStateBase() {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleProcExchangeStateBase);

  virtual bool ShouldGraph(int graphIndex) const = 0;
  virtual int LengthSamples(int graphIndex) const = 0;
  virtual float PositionNormalized(int graphIndex) const = 0;
};

struct alignas(FBSIMDAlign) FBModuleProcSingleExchangeState:
public FBModuleProcExchangeStateBase
{
  int lengthSamples = {};
  int positionSamples = {};
  ~FBModuleProcSingleExchangeState() = default;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleProcSingleExchangeState);

  int LengthSamples(int /*graphIndex*/) const override 
  { return lengthSamples; }
  bool ShouldGraph(int /*graphIndex*/) const override 
  { return boolIsActive != 0 && positionSamples < lengthSamples; }
  float PositionNormalized(int /*graphIndex*/) const override 
  { return positionSamples / static_cast<float>(lengthSamples); }
};

template <int N>
struct alignas(FBSIMDAlign) FBModuleProcMultiExchangeState:
public FBModuleProcExchangeStateBase
{
  std::array<int, N> lengthSamples = {};
  std::array<int, N> positionSamples = {};
  ~FBModuleProcMultiExchangeState() = default;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleProcMultiExchangeState);

  int LengthSamples(int graphIndex) const override
  { return lengthSamples[graphIndex]; }
  bool ShouldGraph(int graphIndex) const override 
  { return boolIsActive != 0 && positionSamples[graphIndex] < lengthSamples[graphIndex]; }
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