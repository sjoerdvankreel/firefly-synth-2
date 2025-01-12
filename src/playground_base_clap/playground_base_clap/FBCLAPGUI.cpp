#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

bool
FBCLAPPlugin::guiCanResize() const noexcept
{
  return true;
}

bool
FBCLAPPlugin::implementsGui() const noexcept
{
  return true;
}

bool
FBCLAPPlugin::guiSetScale(double scale) noexcept
{
  _gui->SetContentScaleFactor((float)scale);
  return true;
}

bool
FBCLAPPlugin::guiShow() noexcept
{
  if (!_gui)
    return false;
  _gui->SetVisible(true);
  return true;
}

bool 
FBCLAPPlugin::guiHide() noexcept 
{
  if (!_gui)
    return false;
  _gui->SetVisible(false);
  return true;
}

void
FBCLAPPlugin::guiDestroy() noexcept 
{
  if (!_gui)
    return;
  _gui->RemoveFromDesktop();
  _gui.reset();
}

bool
FBCLAPPlugin::guiSetParent(const clap_window* window) noexcept
{
  if (!_gui)
    return false;
  _gui->AddToDesktop(window->ptr);
  return true;
}

bool
FBCLAPPlugin::guiCreate(const char* api, bool isFloating) noexcept
{
  _gui = _topo->static_.guiFactory(_topo.get(), this);
  return true;
}

bool
FBCLAPPlugin::guiSetSize(uint32_t width, uint32_t height) noexcept 
{
  if (!_gui)
    return false;
  guiAdjustSize(&width, &height);
  _gui->SetScaledSize(width, height);
  return true;
}

bool
FBCLAPPlugin::guiGetSize(uint32_t* width, uint32_t* height) noexcept 
{
  if (!_gui)
    return false;
  *width = _gui->GetScaledWidth();
  *height = _gui->GetScaledHeight();
  guiAdjustSize(width, height);
  return true;
}

bool
FBCLAPPlugin::guiAdjustSize(uint32_t* width, uint32_t* height) noexcept 
{
  if (!_gui)
    return false;
  int minW = _gui->GetMinScaledWidth();
  int maxW = _gui->GetMaxScaledWidth();
  int arW = _gui->GetAspectRatioWidth();
  int arH = _gui->GetAspectRatioHeight();
  *width = std::clamp((int)*width, minW, maxW);
  *height = *width * arH / arW;
  return true;
}

bool
FBCLAPPlugin::guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept 
{
  hints->preserve_aspect_ratio = true;
  hints->can_resize_vertically = true;
  hints->can_resize_horizontally = true;
  hints->aspect_ratio_width = _gui->GetAspectRatioWidth();
  hints->aspect_ratio_height = _gui->GetAspectRatioHeight();
  return true;
}

bool
FBCLAPPlugin::guiIsApiSupported(const char* api, bool isFloating) noexcept 
{
  if (std::string(api) == CLAP_WINDOW_API_X11) return true;
  if (std::string(api) == CLAP_WINDOW_API_WIN32) return true;
  if (std::string(api) == CLAP_WINDOW_API_COCOA) return true;
  return false;
}