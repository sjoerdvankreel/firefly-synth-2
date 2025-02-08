#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBProcStateContainer;

class FBScalarStateContainer final
{
  void* _rawState;
  std::vector<float*> _params;
  void (*_freeRawState)(void*);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBScalarStateContainer);
  FBScalarStateContainer(FBRuntimeTopo const& topo);
  ~FBScalarStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBProcStateContainer const& proc);
  void CopyFrom(FBScalarStateContainer const& scalar);
  std::vector<float*> const& Params() const { return _params; }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  template <class T> T* As() { return static_cast<T*>(_rawState); }
  template <class T> T const* As() const { return static_cast<T const*>(_rawState); }
};