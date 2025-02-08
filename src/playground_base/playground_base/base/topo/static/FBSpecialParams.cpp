#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/static/FBSpecialParams.hpp>

FBStaticParam const& 
FBSpecialParam::ParamTopo(FBStaticTopo const& topo) const
{
  return topo.modules[moduleIndex].params[paramIndex];
}