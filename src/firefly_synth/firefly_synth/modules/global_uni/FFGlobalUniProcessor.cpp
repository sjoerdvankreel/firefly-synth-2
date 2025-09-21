#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

void 
FFGlobalUniProcessor::BeginBlock(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.globalUni[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GlobalUni];
  float typeNorm = params.block.type[0].Value();
  _type = topo.NormalizedToListFast<FFGlobalUniType>((int)FFGlobalUniParam::Type, typeNorm);
}