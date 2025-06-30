#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContainer.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FBPlugGUIContainer::
~FBPlugGUIContainer() {}

FBPlugGUIContainer::
FBPlugGUIContainer(FBHostGUIContext* hostContext):
FBPlugGUIContext(hostContext),
_gui(hostContext->Topo()->static_.guiFactory(hostContext))
{
  FB_LOG_ENTRY_EXIT();
  setOpaque(true);
  setVisible(true);
  
  FB_LOG_INFO("Calculating GUI size.");
  int plugWidth = hostContext->Topo()->static_.guiWidth;
  int plugHeight = GetHeightForAspectRatio(plugWidth);
  FB_LOG_INFO("Calculated GUI size.");

  FB_LOG_INFO("Adjusting JUCE GUI size.");
  setSize(plugWidth, plugHeight);
  FB_LOG_INFO("Adjusted JUCE GUI size.");

  FB_LOG_INFO("Adjusting plug GUI size.");
  _gui->setSize(plugWidth, plugHeight);
  FB_LOG_INFO("Adjusted plug GUI size.");

  FB_LOG_INFO("Showing plug GUI.");
  addAndMakeVisible(_gui.get());
  FB_LOG_INFO("Showed plug GUI.");
}

void 
FBPlugGUIContainer::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

void
FBPlugGUIContainer::RemoveFromDesktop()
{
  _gui->HostContext()->UndoState().DeactivateNow();
  removeFromDesktop();
}

void
FBPlugGUIContainer::UpdateExchangeState()
{
  _gui->UpdateExchangeState();
}

void
FBPlugGUIContainer::SetVisible(bool visible)
{
  setVisible(visible);
}

void
FBPlugGUIContainer::AddToDesktop(void* parent)
{
  addToDesktop(0, parent);
}

void
FBPlugGUIContainer::SetAudioParamNormalizedFromHost(int index, double normalized)
{
  _gui->AudioParamNormalizedChangedFromHost(index, normalized);
}

void
FBPlugGUIContainer::RequestRescale(double combinedScale)
{
  auto hostSize = GetHostSize();
  setSize(hostSize.first, hostSize.second);
  _gui->setTransform(AffineTransform::scale(static_cast<float>(combinedScale)));
}