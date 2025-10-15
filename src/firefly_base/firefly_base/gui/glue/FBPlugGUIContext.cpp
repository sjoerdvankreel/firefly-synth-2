#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>

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

int
FBPlugGUIContext::ClampHostWidthForScale(int width) const
{
  auto const& topo = _hostContext->Topo()->static_;
  double minW = topo->guiWidth * _hostContext->GetUserScaleMin() * _systemScale;
  double maxW = topo->guiWidth * _hostContext->GetUserScaleMax() * _systemScale;
  return static_cast<int>(std::round(std::clamp(static_cast<double>(width), minW, maxW)));
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
  double userScalePlain = (static_cast<double>(width) / topo->guiWidth) / _systemScale;
  _hostContext->SetUserScalePlain(userScalePlain);
  RequestRescale(CombinedScale());
}