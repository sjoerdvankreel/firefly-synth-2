#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

// TODO moduleexch
class FBActiveExchangeState final
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

  FB_NOCOPY_MOVE_DEFCTOR(FBActiveExchangeState);
  explicit FBActiveExchangeState(bool* global);
  explicit FBActiveExchangeState(std::array<bool*, FBMaxVoices> const& voice);
};

inline FBActiveExchangeState::
FBActiveExchangeState(bool* global) :
_isGlobal(true),
_global(global) {}

inline FBActiveExchangeState::
FBActiveExchangeState(std::array<bool*, FBMaxVoices> const& voice) :
_isGlobal(false),
_voice(voice) {}

inline bool*
FBActiveExchangeState::Global()
{
  assert(IsGlobal());
  return _global;
}

inline bool const*
FBActiveExchangeState::Global() const
{
  assert(IsGlobal());
  return _global;
}

inline std::array<bool*, FBMaxVoices>&
FBActiveExchangeState::Voice()
{
  assert(!IsGlobal());
  return _voice;
}

inline std::array<bool*, FBMaxVoices> const&
FBActiveExchangeState::Voice() const
{
  assert(!IsGlobal());
  return _voice;
}