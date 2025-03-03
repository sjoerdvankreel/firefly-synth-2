#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

FBRuntimeGUIParam::
FBRuntimeGUIParam(
  FBStaticModule const& staticModule,
  FBStaticGUIParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(FBStaticGUIParam(staticParam)) {}