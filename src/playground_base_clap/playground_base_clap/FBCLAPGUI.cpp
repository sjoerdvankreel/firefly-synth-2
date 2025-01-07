#include <playground_base_clap/FBCLAPPlugin.hpp>

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
}

bool 
FBCLAPPlugin::guiHide() noexcept 
{
}

void
FBCLAPPlugin::guiDestroy() noexcept 
{
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
FBCLAPPlugin::guiCreate(const char* api, bool isFloating) noexcept 
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
}

bool
FBCLAPPlugin::guiGetPreferredApi(const char** api, bool* isFloating) noexcept 
{
}