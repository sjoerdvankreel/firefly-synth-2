#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>

struct FBStaticModule;

struct FBRuntimeParam final
{
  int const runtimeModuleIndex;
  int const staticModuleIndex;
  int const staticModuleSlot;
  int const staticIndex;
  int const staticSlot;
  FBStaticParam const static_;

  std::string const longName;
  std::string const shortName;
  std::string const id;
  int const tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& staticModule, 
    FBStaticParam const& staticParam,
    int runtimeModuleIndex,
    int staticModuleIndex, int staticModuleSlot,
    int staticIndex, int staticSlot);    
};