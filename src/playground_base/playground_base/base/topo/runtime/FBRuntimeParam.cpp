#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(staticParam) {}