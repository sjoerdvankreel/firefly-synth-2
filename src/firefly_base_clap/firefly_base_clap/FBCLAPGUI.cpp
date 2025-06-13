#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContainer.hpp>

#include <clap/helpers/host-proxy.hxx>
#include <juce_gui_basics/juce_gui_basics.h>

#if (defined __linux__) || (defined  __FreeBSD__)
#include <juce_events/native/juce_EventLoopInternal_linux.h>
#endif

using namespace juce;

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
FBCLAPPlugin::guiShow() noexcept
{
  return FBWithLogException([this]()
  {
    if (!_gui)
      return false;
    _gui->SetVisible(true);
    return true;
  });
}

bool 
FBCLAPPlugin::guiHide() noexcept 
{
  return FBWithLogException([this]()
  {
    if (!_gui)
      return false;
    _gui->SetVisible(false);
    return true;
  });
}

bool
FBCLAPPlugin::guiSetScale(double scale) noexcept
{
  return FBWithLogException([this, scale]()
  {
    _gui->SetSystemScale(scale);
    auto hostSize = _gui->GetHostSize();
    _host.guiRequestResize(hostSize.first, hostSize.second);
    return true;
  });
}

void
FBCLAPPlugin::guiDestroy() noexcept 
{
  FB_LOG_ENTRY_EXIT();
  FBWithLogException([this]()
  {
    if (!_gui)
      return;
    _gui->RemoveFromDesktop();
    _gui.reset();
#if (defined __linux__) || (defined  __FreeBSD__)
    for (int fd : LinuxEventLoopInternal::getRegisteredFds())
      _host.posixFdSupportUnregister(fd);
#endif
  });
}

bool
FBCLAPPlugin::guiSetParent(const clap_window* window) noexcept
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, window]()
  {
    if (!_gui)
      return false;
    _gui->AddToDesktop(window->ptr);
#if (defined __linux__) || (defined  __FreeBSD__)
    for (int fd : LinuxEventLoopInternal::getRegisteredFds())
      _host.posixFdSupportRegister(fd, CLAP_POSIX_FD_READ);
#endif
    return true;
  });
}

bool
FBCLAPPlugin::guiCreate(const char* /*api*/, bool /*isFloating*/) noexcept
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this]()
  {
    _gui = std::make_unique<FBPlugGUIContainer>(this);
    return true;
  });
}

bool
FBCLAPPlugin::guiSetSize(uint32_t width, uint32_t height) noexcept 
{
  return FBWithLogException([this, &width, &height]()
  {
    if (!_gui)
      return false;
    guiAdjustSize(&width, &height);
    _gui->SetUserScaleByHostWidth(width);
    return true;
  });
}

bool
FBCLAPPlugin::guiGetSize(uint32_t* width, uint32_t* height) noexcept 
{
  return FBWithLogException([this, width, height]()
  {
    if (!_gui)
      return false;
    auto hostSize = _gui->GetHostSize();
    *width = hostSize.first;
    *height = hostSize.second;
    return true;
  });
}

bool
FBCLAPPlugin::guiAdjustSize(uint32_t* width, uint32_t* height) noexcept 
{
  return FBWithLogException([this, width, height]()
  {
    if (!_gui)
      return false;
    *width = _gui->ClampHostWidthForScale(*width);
    *height = _gui->GetHeightForAspectRatio(*width);
    return true;
  });
}

bool
FBCLAPPlugin::guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept 
{
  hints->preserve_aspect_ratio = true;
  hints->can_resize_vertically = true;
  hints->can_resize_horizontally = true;
  hints->aspect_ratio_width = _topo->static_.guiAspectRatioWidth;
  hints->aspect_ratio_height = _topo->static_.guiAspectRatioHeight;
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