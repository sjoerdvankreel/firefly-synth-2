#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <string>
#include <vector>

struct FBStaticModule;

struct FBRuntimeModule final
{
  int staticIndex;
  int staticSlot;
  std::string name;
  std::vector<FBRuntimeParam> params;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
  FBRuntimeModule(
    FBStaticModule const& staticModule,
    int runtimeIndex, int runtimeParamStart,
    int staticIndex, int staticSlot);
};