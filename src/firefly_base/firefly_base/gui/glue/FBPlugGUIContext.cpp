#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <cmath>
#include <algorithm>

FBPlugGUIContext::
FBPlugGUIContext(FBHostGUIContext* const hostContext):
_hostContext(hostContext) 
{
  FB_LOG_ENTRY_EXIT();
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
  return width * topo->guiAspectRatioHeight / topo->guiAspectRatioWidth;
}

double
FBPlugGUIContext::CombinedScale() const
{
  return _systemScale * _hostContext->GetUserScalePlain();
}

void
FBPlugGUIContext::ClampHostSizeForScale(int& width, int& height) const
{
  auto const& topo = _hostContext->Topo()->static_;
  double ratioW = (double)topo->guiAspectRatioWidth;
  double ratioH = (double)topo->guiAspectRatioHeight;
  double minW = topo->guiWidth * _hostContext->GetUserScaleMin() * _systemScale;
  double maxW = topo->guiWidth * _hostContext->GetUserScaleMax() * _systemScale;
  double requestedWByW = (double)width;
  double requestedWByH = (double)height * ratioW / ratioH;
  double dw = std::min(requestedWByW, requestedWByH);
  double dh = dw * ratioH / ratioW;
  dw = std::clamp(dw, minW, maxW);
  dh = dw * ratioH / ratioW;
  if (dw < minW)
  {
    dw = minW;
    dh = dw * ratioH / ratioW;
  }
  if (dw > maxW)
  {
    dw = maxW;
    dh = dw * ratioH / ratioW;
  }
  width = (int)std::round(dw);
  height = (int)std::round(dh);
}

std::pair<int, int>
FBPlugGUIContext::GetHostSize() const
{
  auto const& topo = _hostContext->Topo()->static_;
  int w = static_cast<int>(std::round(topo->guiWidth * CombinedScale()));
  int h = static_cast<int>(std::round(GetHeightForAspectRatio(topo->guiWidth) * CombinedScale()));
  return { w, h };
}

void
FBPlugGUIContext::SetUserScaleByHostWidth(int width)
{
  auto const& topo = _hostContext->Topo()->static_;
  double userScalePlain = static_cast<double>(width) / topo->guiWidth;
  _hostContext->SetUserScalePlain(userScalePlain / _systemScale);
  RequestRescale(CombinedScale());
}