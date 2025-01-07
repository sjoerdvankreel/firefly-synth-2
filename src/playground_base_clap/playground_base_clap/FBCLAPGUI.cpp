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
  return false; // TODO scaling
}

bool
FBCLAPPlugin::guiShow() noexcept
{
  _gui->setVisible(true);
  return true;
}

bool 
FBCLAPPlugin::guiHide() noexcept 
{
  _gui->setVisible(false);
  return true;
}

void
FBCLAPPlugin::guiDestroy() noexcept 
{
  _gui->removeFromDesktop();
  _gui.reset();
}

bool
FBCLAPPlugin::guiCreate(const char* api, bool isFloating) noexcept
{
  _gui = _topo->static_.guiFactory(_topo.get(), this);
  return true;
}

bool
FBCLAPPlugin::guiSetParent(const clap_window* window) noexcept 
{
}

bool
FBCLAPPlugin::guiSetSize(uint32_t width, uint32_t height) noexcept 
{
}

bool
FBCLAPPlugin::guiGetSize(uint32_t* width, uint32_t* height) noexcept 
{
}

bool
FBCLAPPlugin::guiAdjustSize(uint32_t* width, uint32_t* height) noexcept 
{
}

bool
FBCLAPPlugin::guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept 
{
}

bool
FBCLAPPlugin::guiIsApiSupported(const char* api, bool isFloating) noexcept 
{
  if (std::string(api) == CLAP_WINDOW_API_X11) return true;
  if (std::string(api) == CLAP_WINDOW_API_WIN32) return true;
  if (std::string(api) == CLAP_WINDOW_API_COCOA) return true;
  return false;
}