#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

enum class FBExchangeParamType
{
  Voice,
  Global
};

class FBExchangeParamState final
{
  friend class FBHostProcessor;
  friend class FBExchangeStateContainer;

  FBExchangeParamType _type;
  union
  {
    float* _global;
    std::array<float, FBMaxVoices>* _voice;
  };

  float* Global();
  std::array<float, FBMaxVoices>& Voice();

public:
  FB_COPY_MOVE_DEFCTOR(FBExchangeParamState);
  explicit FBExchangeParamState(float* global);
  explicit FBExchangeParamState(std::array<float, FBMaxVoices>* voice);

  float const* Global() const;
  std::array<float, FBMaxVoices> const& Voice() const;
  FBExchangeParamType Type() const { return _type; }
  bool IsGlobal() const { return Type() == FBExchangeParamType::Global; }
};

inline FBExchangeParamState::
FBExchangeParamState(float* global):
_type(FBExchangeParamType::Global),
_global(global) {}

inline FBExchangeParamState::
FBExchangeParamState(std::array<float, FBMaxVoices>* voice):
_type(FBExchangeParamType::Voice),
_voice(voice) {}

inline float*
FBExchangeParamState::Global()
{
  assert(Type() == FBExchangeParamType::Global);
  return _global;
}

inline float const*
FBExchangeParamState::Global() const
{
  assert(Type() == FBExchangeParamType::Global);
  return _global;
}

inline std::array<float, FBMaxVoices>&
FBExchangeParamState::Voice()
{
  assert(Type() == FBExchangeParamType::Voice);
  return *_voice;
}

inline std::array<float, FBMaxVoices> const&
FBExchangeParamState::Voice() const
{
  assert(Type() == FBExchangeParamType::Voice);
  return *_voice;
}