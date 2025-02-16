#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticAudioParam.hpp>
#include <playground_base/base/topo/runtime/FBParamTopoIndices.hpp>

#include <string>

struct FBStaticModule;

struct FBRuntimeAudioParam final
{
  int runtimeModuleIndex;
  int runtimeParamIndex;
  FBStaticAudioParam static_;
  FBParamTopoIndices topoIndices;

  std::string longName;
  std::string shortName;
  std::string tooltip;
  std::string id;
  int tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeAudioParam);
  FBRuntimeAudioParam(
    FBStaticModule const& staticModule,
    FBStaticAudioParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};