#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <vector>

struct FBRuntimeTopo;
class FBHostGUIContext;
class FBProcStateContainer;

class FBScalarStateContainer final
{
  std::vector<double*> _params;
  void* _rawState;
  void (*_freeRawState)(void*);

public:
  FBScalarStateContainer(FBRuntimeTopo const& topo);
  ~FBScalarStateContainer() { if(_rawState) _freeRawState(_rawState); }
  FBScalarStateContainer(FBScalarStateContainer&&) noexcept;
  FBScalarStateContainer& operator=(FBScalarStateContainer&&) noexcept;
  FBScalarStateContainer(FBScalarStateContainer const&) = delete;
  FBScalarStateContainer& operator=(FBScalarStateContainer const&) = delete;

  void CopyTo(FBHostGUIContext* context) const;
  void CopyFrom(FBHostGUIContext const* context);
  void CopyFrom(FBProcStateContainer const& proc);
  void CopyFrom(FBScalarStateContainer const& scalar);

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  std::vector<double*> const& Params() const { return _params; }
  template <class T> T* As() { return static_cast<T*>(_rawState); }
  template <class T> T const* As() const { return static_cast<T const*>(_rawState); }
};