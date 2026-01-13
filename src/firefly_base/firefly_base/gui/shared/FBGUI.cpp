#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

static std::unique_ptr<FBLookAndFeel> _defaultLookAndFeel = {};

void
FBGUITerminate()
{
  FB_LOG_INFO("Terminating GUI.");
  LookAndFeel::setDefaultLookAndFeel(nullptr);
  _defaultLookAndFeel.reset();
  FB_LOG_INFO("Terminating JUCE GUI.");
  shutdownJuce_GUI();
  FB_LOG_INFO("Terminated JUCE GUI.");
  FB_LOG_INFO("Terminated GUI.");
}

void
FBGUIInit()
{
  FB_LOG_INFO("Initializing GUI.");
  FB_LOG_INFO("Initializing JUCE GUI.");
  initialiseJuce_GUI();
  FB_LOG_INFO("Initialized JUCE GUI.");  

  // We still need this even with per-pluggui look and feel.
  // It controls some stuff during setting up the gui like createComboBoxTextBox
  // which is fortunately not theme-dependent.
  _defaultLookAndFeel = std::make_unique<FBLookAndFeel>();
  LookAndFeel::setDefaultLookAndFeel(_defaultLookAndFeel.get());
  FB_LOG_INFO("Initialized GUI.");
}

FBLookAndFeel* 
FBGetLookAndFeelFor(FBPlugGUI const* c)
{
  return c->LookAndFeel();
}

FBLookAndFeel*
FBGetLookAndFeelFor(Component const* c)
{
  if (auto container = dynamic_cast<FBPlugGUI const*>(c))
    return container->LookAndFeel();
  if (auto container = c->findParentComponentOfClass<FBPlugGUI>())
    return container->LookAndFeel();
  FB_ASSERT(false);
  return nullptr;
}