#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

class FBExchangeParamState final
{
  friend class FBHostProcessor;
  friend class FBExchangeStateContainer;

  bool _isGlobal;
  union
  {
    float* _global;
    std::array<float, FBMaxVoices>* _voice;
  };

  float* Global();
  std::array<float, FBMaxVoices>& Voice();

public:
  float const* Global() const;
  bool IsGlobal() const { return _isGlobal; }
  std::array<float, FBMaxVoices> const& Voice() const;

  FB_COPY_MOVE_DEFCTOR(FBExchangeParamState);
  explicit FBExchangeParamState(float* global);
  explicit FBExchangeParamState(std::array<float, FBMaxVoices>* voice);
};

inline FBExchangeParamState::
FBExchangeParamState(float* global):
_isGlobal(true),
_global(global) {}

inline FBExchangeParamState::
FBExchangeParamState(std::array<float, FBMaxVoices>* voice):
_isGlobal(false),
_voice(voice) {}

inline float*
FBExchangeParamState::Global()
{
  assert(IsGlobal());
  return _global;
}

inline float const*
FBExchangeParamState::Global() const
{
  assert(IsGlobal());
  return _global;
}

inline std::array<float, FBMaxVoices>&
FBExchangeParamState::Voice()
{
  assert(!IsGlobal());
  return *_voice;
}

inline std::array<float, FBMaxVoices> const&
FBExchangeParamState::Voice() const
{
  assert(!IsGlobal());
  return *_voice;
}