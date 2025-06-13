#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>

class FBParamExchangeState final
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

  FB_NOCOPY_MOVE_DEFCTOR(FBParamExchangeState);
  explicit FBParamExchangeState(float* global);
  explicit FBParamExchangeState(std::array<float, FBMaxVoices>* voice);
};

inline FBParamExchangeState::
FBParamExchangeState(float* global):
_isGlobal(true),
_global(global) {}

inline FBParamExchangeState::
FBParamExchangeState(std::array<float, FBMaxVoices>* voice):
_isGlobal(false),
_voice(voice) {}

inline float*
FBParamExchangeState::Global()
{
  FB_ASSERT(IsGlobal());
  return _global;
}

inline float const*
FBParamExchangeState::Global() const
{
  FB_ASSERT(IsGlobal());
  return _global;
}

inline std::array<float, FBMaxVoices>&
FBParamExchangeState::Voice()
{
  FB_ASSERT(!IsGlobal());
  return *_voice;
}

inline std::array<float, FBMaxVoices> const&
FBParamExchangeState::Voice() const
{
  FB_ASSERT(!IsGlobal());
  return *_voice;
}