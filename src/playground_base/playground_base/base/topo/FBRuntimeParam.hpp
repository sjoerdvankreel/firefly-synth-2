#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>
#include <playground_base/base/topo/FBParamTopoIndices.hpp>

#include <string>

struct FBStaticModule;

struct FBRuntimeParam final
{
  int runtimeModuleIndex;
  int runtimeParamIndex;
  FBStaticParam static_;
  FBParamTopoIndices topoIndices;

  std::string longName;
  std::string shortName;
  std::string id;
  int tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& staticModule,
    FBStaticParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};