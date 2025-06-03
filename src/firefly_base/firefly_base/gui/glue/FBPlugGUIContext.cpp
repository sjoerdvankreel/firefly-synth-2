#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>

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
  auto const& topo = _hostContext->Topo()->static_;
  return width * topo.guiAspectRatioHeight / topo.guiAspectRatioWidth;
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
  auto const& topo = _hostContext->Topo()->static_;
  double minW = topo.guiWidth * UserScaleParam().Linear().min * _systemScale;
  double maxW = topo.guiWidth * UserScaleParam().Linear().max * _systemScale;
  return static_cast<int>(std::round(std::clamp(static_cast<double>(width), minW, maxW)));
}

std::pair<int, int>
FBPlugGUIContext::GetHostSize() const
{
  auto const& topo = _hostContext->Topo()->static_;
  int w = static_cast<int>(std::round(topo.guiWidth * CombinedScale()));
  int h = static_cast<int>(std::round(GetHeightForAspectRatio(topo.guiWidth) * CombinedScale()));
  return { w, h };
}

void
FBPlugGUIContext::SetUserScaleByHostWidth(int width)
{
  auto const& topo = _hostContext->Topo()->static_;
  double userScalePlain = (static_cast<double>(width) / topo.guiWidth) / _systemScale;
  *UserScaleSpecial().state = UserScaleParam().NonRealTime().PlainToNormalized(userScalePlain);
  RequestRescale(CombinedScale());
}