#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <string>

struct FBStaticTopo;
struct FBStaticModule;

std::string
MakeRuntimeParamLongName(
  FBStaticTopo const& topo,
  FBStaticModule const& module,
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices);

struct FBRuntimeParamBase
{
  int runtimeModuleIndex;
  int runtimeParamIndex;
  FBParamTopoIndices topoIndices;

  std::string longName;
  std::string shortName;
  std::string displayName;
  std::string id;
  std::string staticModuleId;
  int tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParamBase);
  virtual FBStaticParamBase const& Static() const = 0;

  std::string PlainToText(bool io, double plain) const;
  std::string NormalizedToText(bool io, double normalized) const;
  std::string NormalizedToTextWithUnit(bool io, double normalized) const;
  std::optional<double> TextToPlain(bool io, std::string const& text) const;
  std::optional<double> TextToNormalized(bool io, std::string const& text) const;

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

  FBStaticParamBase const& Static() const override { return static_; }
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

  FBStaticParamBase const& Static() const override { return static_; }
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeParam);

  FBRuntimeParam(
    FBStaticTopo const& topo,
    FBStaticModule const& staticModule,
    FBStaticParam const& staticParam,
    FBParamTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeParamIndex);
};