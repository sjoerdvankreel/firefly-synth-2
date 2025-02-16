#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticParamBase.hpp>
#include <playground_base/base/topo/runtime/FBParamTopoIndices.hpp>

#include <string>

struct FBStaticModule;

struct FBRuntimeParamBase
{
  int runtimeModuleIndex;
  int runtimeParamIndex;
  FBParamTopoIndices topoIndices;

  std::string longName;
  std::string shortName;
  std::string tooltip;
  std::string id;
  int tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParamBase);
  FBRuntimeParamBase(
    FBStaticModule const& staticModule,
    FBStaticParamBase const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};