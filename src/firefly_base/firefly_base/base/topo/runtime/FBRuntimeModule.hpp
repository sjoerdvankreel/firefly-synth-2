#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeCVOutput.hpp>

#include <string>
#include <vector>

struct FBStaticTopo;
struct FBStaticModule;

struct FBRuntimeModule final
{
  std::string name;
  int runtimeModuleIndex;
  FBTopoIndices topoIndices;
  std::vector<FBRuntimeParam> params;
  std::vector<FBRuntimeGUIParam> guiParams;
  std::vector<FBRuntimeCVOutput> cvOutputs;

  FBRuntimeModule(
    FBStaticTopo const& topo, FBStaticModule const& staticModule,
    FBTopoIndices const& topoIndices, int runtimeIndex, 
    int runtimeParamStart, int runtimeGUIParamStart, 
    int runtimeCVOutputStart);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
};