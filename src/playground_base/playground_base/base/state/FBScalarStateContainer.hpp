#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBProcStatePtrs;

class FBScalarStateContainer final
{
  void* _rawState;
  std::vector<float*> _params;
  void (*_freeRawState)(void*);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBScalarStateContainer);
  FBScalarStateContainer(FBRuntimeTopo const& topo);
  ~FBScalarStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBProcStatePtrs const& proc);
  void CopyFrom(FBScalarStateContainer const& scalar);
  std::vector<float*> const& Params() const { return _params; }
};