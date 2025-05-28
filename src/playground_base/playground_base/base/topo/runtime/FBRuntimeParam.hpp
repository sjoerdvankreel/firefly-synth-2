#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/runtime/FBTopoIndices.hpp>

#include <string>

struct FBStaticModule;

struct FBRuntimeParamBase
{
  int runtimeModuleIndex;
  int runtimeParamIndex;
  FBParamTopoIndices topoIndices;

  std::string longName;
  std::string shortName;
  std::string displayName;
  std::string id;
  int tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParamBase);
  FBRuntimeParamBase(
    FBStaticModule const& staticModule,
    FBStaticParamBase const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};

struct FBRuntimeGUIParam final:
public FBRuntimeParamBase
{
  FBStaticGUIParam static_;
  std::string GetDefaultText() const;
  double DefaultNormalizedByText() const;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeGUIParam);
  FBRuntimeGUIParam(
    FBStaticModule const& staticModule,
    FBStaticGUIParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};

struct FBRuntimeParam final:
public FBRuntimeParamBase
{
  FBStaticParam static_;
  std::string GetDefaultText() const;
  double DefaultNormalizedByText() const;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);
  FBRuntimeParam(
    FBStaticModule const& staticModule,
    FBStaticParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};