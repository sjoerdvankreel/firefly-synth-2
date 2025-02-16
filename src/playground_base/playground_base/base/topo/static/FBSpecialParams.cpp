#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/static/FBSpecialParams.hpp>

FBStaticAudioParam const& 
FBSpecialParam::ParamTopo(FBStaticTopo const& topo) const
{
  return topo.modules[moduleIndex].audioParams[paramIndex];
}