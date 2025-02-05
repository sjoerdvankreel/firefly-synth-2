#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

class FBExchangeActiveState final
{
  friend class FBHostProcessor;
  friend class FBExchangeStateContainer;

  bool _isGlobal;
  union
  {
    bool* _global;
    std::array<bool*, FBMaxVoices> _voice;
  };

  bool* Global();
  std::array<bool*, FBMaxVoices>& Voice();

public:
  bool const* Global() const;
  bool IsGlobal() const { return _isGlobal; }
  std::array<bool*, FBMaxVoices> const& Voice() const;

  FB_COPY_MOVE_DEFCTOR(FBExchangeActiveState);
  explicit FBExchangeActiveState(bool* global);
  explicit FBExchangeActiveState(std::array<bool*, FBMaxVoices> const& voice);
};

inline FBExchangeActiveState::
FBExchangeActiveState(bool* global) :
_isGlobal(true),
_global(global) {}

inline FBExchangeActiveState::
FBExchangeActiveState(std::array<bool*, FBMaxVoices> const& voice) :
_isGlobal(false),
_voice(voice) {}

inline bool*
FBExchangeActiveState::Global()
{
  assert(IsGlobal());
  return _global;
}

inline bool const*
FBExchangeActiveState::Global() const
{
  assert(IsGlobal());
  return _global;
}

inline std::array<bool*, FBMaxVoices>&
FBExchangeActiveState::Voice()
{
  assert(!IsGlobal());
  return _voice;
}

inline std::array<bool*, FBMaxVoices> const&
FBExchangeActiveState::Voice() const
{
  assert(!IsGlobal());
  return _voice;
}