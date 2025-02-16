#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/runtime/FBTopoIndices.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeAudioParam.hpp>

#include <string>
#include <vector>

struct FBStaticModule;

struct FBRuntimeModule final
{
  std::string name;
  int runtimeModuleIndex;
  FBTopoIndices topoIndices;
  std::vector<FBRuntimeAudioParam> audioParams;

  FBRuntimeModule(
    FBStaticModule const& staticModule,
    FBTopoIndices const& topoIndices,
    int runtimeIndex, int runtimeParamStart);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
};