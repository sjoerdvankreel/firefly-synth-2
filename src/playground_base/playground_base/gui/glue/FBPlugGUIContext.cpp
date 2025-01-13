#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/base/topo/FBStaticGUITopo.hpp>

#include <cmath>

FBPlugGUIContext::
FBPlugGUIContext(FBStaticGUITopo const* topo):
_topo(topo) {}

float 
FBPlugGUIContext::CombinedScale() const
{
  return _userScale * _systemScale;
}

void 
FBPlugGUIContext::SetSystemScale(float scale)
{
  _systemScale = scale;
  RequestRescale(CombinedScale());
}

void 
FBPlugGUIContext::SetUserScaleByHostWidth(int width)
{
  _userScale = ((float)width / (float)_topo->plugWidth) / _systemScale;
  RequestRescale(CombinedScale());
}

std::pair<int, int>
FBPlugGUIContext::GetHostSize() const
{
  int w = (int)std::round(_topo->plugWidth * CombinedScale());
  int h = (int)std::round(_topo->plugWidth * _topo->aspectRatioHeight / _topo->aspectRatioWidth * CombinedScale());
  return { w, h };
}