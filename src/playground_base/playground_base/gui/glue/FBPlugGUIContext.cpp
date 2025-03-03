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
FBPlugGUIContext::SetSystemScale(float scale)
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

float
FBPlugGUIContext::CombinedScale() const
{
  float userScaleNorm = *UserScaleSpecial().state;
  float userScalePlain = UserScaleParam().LinearRealTime().NormalizedToPlain(userScaleNorm);
  return _systemScale * userScalePlain;
}

int
FBPlugGUIContext::ClampHostWidthForScale(int width) const
{
  auto const& topoGUI = _hostContext->Topo()->static_.gui;
  float minW = topoGUI.plugWidth * UserScaleParam().LinearRealTime().min * _systemScale;
  float maxW = topoGUI.plugWidth * UserScaleParam().LinearRealTime().max * _systemScale;
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
  float userScalePlain = ((float)width / (float)topoGUI.plugWidth) / _systemScale;
  *UserScaleSpecial().state = UserScaleParam().NonRealTime().PlainToNormalized(userScalePlain);
  RequestRescale(CombinedScale());
}