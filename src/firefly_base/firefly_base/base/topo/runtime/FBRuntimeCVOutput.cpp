#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeCVOutput.hpp>

std::string
FBMakeRuntimeCVOutputName(
  FBStaticTopo const& topo,
  FBStaticModule const& module,
  FBStaticCVOutput const& cvOutput,
  FBCVOutputTopoIndices const& indices,
  bool onNote)
{
  auto const& name = module.matrixName.size() ? module.matrixName : module.name;
  std::string result = FBMakeRuntimeModuleShortName(
    topo, name, module.slotCount, indices.module.slot, 
    module.slotFormatter, module.slotFormatterOverrides);
  if (module.cvOutputs.size() != 1)
  {
    if (cvOutput.slotFormatter != nullptr)
      result += cvOutput.slotFormatter(topo, indices.module.slot, indices.cvOutput.slot);
    else
    {
      result += " ";
      result += cvOutput.name;
      if (cvOutput.slotCount != 1)
        result += " " + std::to_string(indices.cvOutput.slot + 1);
    }
  }
  else if (cvOutput.slotCount != 1)
  {
    result += " " + module.cvOutputs[0].name + " " + std::to_string(indices.cvOutput.slot + 1);
  }
  std::string prefix = onNote ? (FBOnNotePrefix + " ") : "";
  return prefix + result;
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
name(FBMakeRuntimeCVOutputName(topo, staticModule, staticCVOutput, topoIndices, false)),
staticModuleId(staticModule.id),
tag(FBMakeStableHash(id)) {}