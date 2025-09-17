#pragma once

#include <firefly_base/dsp/shared/FBNoteMatrix.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/exchange/FBParamExchangeState.hpp>
#include <firefly_base/base/state/exchange/FBModuleExchangeState.hpp>

#include <array>
#include <vector>
#include <memory>
#include <compare>

struct FBRuntimeTopo;
struct FBRuntimeParam;

struct alignas(FBSIMDAlign) FBHostExchangeState final
{
  float bpm = {};
  float sampleRate = {};
  FBNoteMatrix<float> noteMatrix = {};
};

struct FBParamActiveExchangeState final
{
  bool active = {};
  float minValue = {};
  float maxValue = {};
  bool operator==(FBParamActiveExchangeState const&) const = default;
};

class FBExchangeStateContainer final
{
  friend class FBHostProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);

  FBHostExchangeState* _host = {};
  std::vector<FBParamExchangeState> _params = {};
  std::array<FBVoiceInfo, FBMaxVoices>* _voices = {};
  std::vector<std::unique_ptr<FBModuleExchangeState>> _modules = {};

  FBHostExchangeState* Host() { return _host; }
  std::vector<FBParamExchangeState>& Params() { return _params; }
  std::array<FBVoiceInfo, FBMaxVoices>& Voices() { return *_voices; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBExchangeStateContainer);
  FBExchangeStateContainer(FBRuntimeTopo const& topo);
  ~FBExchangeStateContainer() { _freeRawState(_rawState); }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  template <class T> T* As() { return static_cast<T*>(Raw()); }
  template <class T> T const* As() const { return static_cast<T const*>(Raw()); }

  FBHostExchangeState const* Host() const { return _host; }
  std::vector<FBParamExchangeState> const& Params() const { return _params; }
  std::array<FBVoiceInfo, FBMaxVoices> const& Voices() const { return *_voices; }
  FBParamActiveExchangeState GetParamActiveState(FBRuntimeParam const* param) const;
  std::vector<std::unique_ptr<FBModuleExchangeState>> const& Modules() const { return _modules; }
};