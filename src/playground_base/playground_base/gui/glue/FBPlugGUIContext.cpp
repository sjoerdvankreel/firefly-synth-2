#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGUIState.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <cmath>
#include <algorithm>

FBPlugGUIContext::
FBPlugGUIContext(FBHostGUIContext* const hostContext):
_hostContext(hostContext) {}

void
FBPlugGUIContext::SetSystemScale(float scale)
{
  _systemScale = scale;
  RequestRescale(CombinedScale());
}

float 
FBPlugGUIContext::CombinedScale() const
{
  return _hostContext->GUIState()->userScale * _systemScale;
}

int
FBPlugGUIContext::GetHeightForAspectRatio(int width) const
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  return width * topoGUI.aspectRatioHeight / topoGUI.aspectRatioWidth;
}

int
FBPlugGUIContext::ClampHostWidthForScale(int width) const
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  float minW = topoGUI.plugWidth * topoGUI.minUserScale * _systemScale;
  float maxW = topoGUI.plugWidth * topoGUI.maxUserScale * _systemScale;
  return (int)std::round(std::clamp((float)width, minW, maxW));
}

std::pair<int, int>
FBPlugGUIContext::GetHostSize() const
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  int w = (int)std::round(topoGUI.plugWidth * CombinedScale());
  int h = (int)std::round(GetHeightForAspectRatio(topoGUI.plugWidth) * CombinedScale());
  return { w, h };
}

void
FBPlugGUIContext::SetUserScaleByHostWidth(int width)
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  _hostContext->GUIState()->userScale = ((float)width / (float)topoGUI.plugWidth) / _systemScale;
  RequestRescale(CombinedScale());
}