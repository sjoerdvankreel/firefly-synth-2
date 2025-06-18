#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <string>

struct FBStaticTopo;
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
    FBStaticTopo const& topo,
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
    FBStaticTopo const& topo,
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
    FBStaticTopo const& topo,
    FBStaticModule const& staticModule,
    FBStaticParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};