#include <firefly_base_vst3/FBVST3Plugin.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>
#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/shared/FBExchangeStateQueue.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>

FBVST3Plugin::
FBVST3Plugin(
  std::unique_ptr<FBStaticTopo>&& topo, 
  MTSClient* mtsClient,
  std::unique_ptr<FBExchangeStateQueueBase>&& exchangeStateQueue):
SingleComponentEffect(),
FBHostContext(std::move(topo), mtsClient, std::move(exchangeStateQueue))
{
  FB_LOG_ENTRY_EXIT();
  processContextRequirements.needTempo();
  processContextRequirements.needContinousTimeSamples();
}

FBVST3Plugin::
~FBVST3Plugin()
{
  FB_LOG_ENTRY_EXIT();
  stopTimer();
}

void
FBVST3Plugin::ResetView()
{
  FB_LOG_ENTRY_EXIT();
  _guiEditor = nullptr;
}

tresult PLUGIN_API
FBVST3Plugin::initialize(FUnknown* context)
{
  FB_LOG_ENTRY_EXIT();
  if (SingleComponentEffect::initialize(context) != kResultTrue)
    return kResultFalse;
  InitializeEditController();
  InitializeAudioEffect();
  startTimerHz(FBGUIFPS);
  return kResultTrue;
}