#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/runtime/FBTopoIndices.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

#include <string>
#include <vector>

struct FBStaticModule;

struct FBRuntimeModule final
{
  std::string name;
  int runtimeModuleIndex;
  FBTopoIndices topoIndices;
  std::vector<FBRuntimeParam> params;
  std::vector<FBRuntimeGUIParam> guiParams;

  FBRuntimeModule(
    FBStaticModule const& staticModule, FBTopoIndices const& topoIndices,
    int runtimeIndex, int runtimeParamStart, int runtimeGUIParamStart);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
};