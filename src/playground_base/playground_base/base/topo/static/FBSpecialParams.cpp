#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBSpecialParams.hpp>

FBStaticParam const& 
FBSpecialParam::ParamTopo(FBStaticTopo const& topo) const
{
  return topo.modules[moduleIndex].params[paramIndex];
}