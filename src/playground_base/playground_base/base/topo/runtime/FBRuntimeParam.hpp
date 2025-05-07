#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParamBase.hpp>

struct FBRuntimeParam final:
public FBRuntimeParamBase
{
  FBStaticParam static_;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& staticModule,
    FBStaticParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};