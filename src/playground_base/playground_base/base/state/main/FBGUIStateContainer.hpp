#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;

class FBGUIStateContainer final
{
  friend class FBPlugGUIContext;

  void* _rawState;
  float* _userScale;
  std::vector<float*> _params;
  void (*_freeRawState)(void*);

  float* UserScale() { return _userScale; }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIStateContainer);
  FBGUIStateContainer(FBRuntimeTopo const& topo);
  ~FBGUIStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBGUIStateContainer const& gui);
  std::vector<float*> const& Params() const { return _params; }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  float const* UserScale() const { return _userScale; }
  template <class T> T* As() { return static_cast<T*>(_rawState); }
  template <class T> T const* As() const { return static_cast<T const*>(_rawState); }
};