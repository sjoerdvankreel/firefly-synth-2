#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <firefly_base/base/topo/static/FBStaticCVOutput.hpp>

#include <string>

struct FBStaticTopo;
struct FBStaticModule;

struct FBRuntimeCVOutput final
{
  int runtimeModuleIndex;
  int runtimeCVOutputIndex;
  FBCVOutputTopoIndices topoIndices;

  std::string id;
  std::string longName;
  std::string shortName;
  std::string staticModuleId;
  int tag;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeCVOutput);
  FBRuntimeCVOutput(
    FBStaticTopo const& topo,
    FBStaticModule const& staticModule,
    FBStaticCVOutput const& staticCVOutput,
    FBCVOutputTopoIndices const& topoIndices,
    int runtimeModuleIndex, int runtimeCVOutputIndex);
};
