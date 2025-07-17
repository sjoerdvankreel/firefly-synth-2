#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeCVOutput.hpp>

std::string
FBMakeRuntimeCVOutputName(
  FBStaticTopo const&,
  FBStaticModule const& module,
  FBStaticCVOutput const& cvOutput,
  FBCVOutputTopoIndices const& indices)
{
  std::string result = module.tabName;
  if (module.slotCount != 1)
    result += " " + std::to_string(indices.module.slot + 1);
  if (module.cvOutputs.size() != 1)
  {
    result += " ";
    result += cvOutput.name;
    if(cvOutput.slotCount != 1)
      result += std::to_string(indices.cvOutput.slot + 1);
  }
  return result;
}

FBRuntimeCVOutput::
FBRuntimeCVOutput(
  FBStaticTopo const& topo,
  FBStaticModule const& staticModule,
  FBStaticCVOutput const& staticCVOutput,
  FBCVOutputTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeCVOutputIndex):
runtimeModuleIndex(runtimeModuleIndex),
runtimeCVOutputIndex(runtimeCVOutputIndex),
topoIndices(topoIndices),
id(FBMakeRuntimeId(staticModule.id, topoIndices.cvOutput.slot, staticCVOutput.id, topoIndices.cvOutput.slot)),
name(FBMakeRuntimeCVOutputName(topo, staticModule, staticCVOutput, topoIndices)),
staticModuleId(staticModule.id),
tag(FBMakeStableHash(id)) {}