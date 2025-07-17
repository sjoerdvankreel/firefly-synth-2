#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeCVOutput.hpp>

std::string
MakeRuntimeCVOutputLongName(
  FBStaticTopo const& topo,
  FBStaticModule const& module,
  FBStaticCVOutput const& cvOutput,
  FBCVOutputTopoIndices const& indices)
{
  auto cvOutputName = FBMakeRuntimeShortName(topo, cvOutput.name, cvOutput.slotCount, indices.cvOutput.slot, {}, false);
  auto moduleName = FBMakeRuntimeShortName(topo, module.name, module.slotCount, indices.module.slot, {}, false);
  return moduleName + " " + cvOutputName;
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
longName(MakeRuntimeCVOutputLongName(topo, staticModule, staticCVOutput, topoIndices)),
shortName(FBMakeRuntimeShortName(topo, staticCVOutput.name, staticCVOutput.slotCount, topoIndices.cvOutput.slot, {}, false)),
staticModuleId(staticModule.id),
tag(FBMakeStableHash(id)) {}
