#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/static/FBSpecialGUIParams.hpp>

FBStaticGUIParam const&
FBSpecialGUIParam::ParamTopo(FBStaticTopo const& topo) const
{
  return topo.modules[moduleIndex].guiParams[paramIndex];
}