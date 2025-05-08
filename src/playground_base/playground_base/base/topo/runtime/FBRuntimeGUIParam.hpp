#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParamBase.hpp>

struct FBRuntimeGUIParam final:
public FBRuntimeParamBase
{
  FBStaticGUIParam static_;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeGUIParam);
  FBRuntimeGUIParam(
    FBStaticModule const& staticModule,
    FBStaticGUIParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};