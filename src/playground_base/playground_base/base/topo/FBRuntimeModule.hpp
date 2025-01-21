#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBTopoIndices.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <string>
#include <vector>

struct FBStaticModule;

struct FBRuntimeModule final
{
  std::string name;
  FBTopoIndices topoIndices;
  std::vector<FBRuntimeParam> params;

  FBRuntimeModule(
    FBStaticModule const& staticModule,
    FBTopoIndices const& topoIndices,
    int runtimeIndex, int runtimeParamStart);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
};