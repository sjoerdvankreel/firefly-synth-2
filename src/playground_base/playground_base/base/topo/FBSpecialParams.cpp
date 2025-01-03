#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBSpecialParams.hpp>
#include <playground_base/base/state/FBGlobalBlockParamState.hpp>

float 
FBSpecialParam::PlainLinear(FBStaticTopo const& topo) const
{
  auto const& module = topo.modules[moduleIndex];
  auto const& param = module.params[paramIndex];
  return param.NormalizedToPlainLinear(state->Value());
}