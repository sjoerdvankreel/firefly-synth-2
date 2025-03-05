#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGUIStateContainer.hpp>

#include <cmath>
#include <algorithm>

FBPlugGUIContext::
FBPlugGUIContext(FBHostGUIContext* const hostContext):
_hostContext(hostContext) {}

FBSpecialGUIParam const& 
FBPlugGUIContext::UserScaleSpecial() const
{
  return _hostContext->GUIState()->Special().userScale;
}

FBStaticGUIParam const&
FBPlugGUIContext::UserScaleParam() const
{
  return UserScaleSpecial().ParamTopo(_hostContext->Topo()->static_);
}

void
FBPlugGUIContext::SetSystemScale(double scale)
{
  _systemScale = scale;
  RequestRescale(CombinedScale());
}

int
FBPlugGUIContext::GetHeightForAspectRatio(int width) const
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  return width * topoGUI.aspectRatioHeight / topoGUI.aspectRatioWidth;
}

double
FBPlugGUIContext::CombinedScale() const
{
  double userScaleNorm = *UserScaleSpecial().state;
  double userScalePlain = UserScaleParam().NonRealTime().NormalizedToPlain(userScaleNorm);
  return _systemScale * userScalePlain;
}

int
FBPlugGUIContext::ClampHostWidthForScale(int width) const
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  double minW = topoGUI.plugWidth * UserScaleParam().Linear().min * _systemScale;
  double maxW = topoGUI.plugWidth * UserScaleParam().Linear().max * _systemScale;
  return static_cast<int>(std::round(std::clamp(static_cast<double>(width), minW, maxW)));
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
  double userScalePlain = (static_cast<double>(width) / topoGUI.plugWidth) / _systemScale;
  *UserScaleSpecial().state = UserScaleParam().NonRealTime().PlainToNormalized(userScalePlain);
  RequestRescale(CombinedScale());
}