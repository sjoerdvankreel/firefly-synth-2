#include <firefly_base_vst3/FBVST3Plugin.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>

FBVST3Plugin::
FBVST3Plugin(std::unique_ptr<FBStaticTopo>&& topo, MTSClient* mtsClient):
SingleComponentEffect(),
FBHostGUIContext(std::move(topo)),
FBHostDSPContext(_topo.get()),
_mtsClient(mtsClient),
_audioClipToDSPCommands(16)
{
  FB_LOG_ENTRY_EXIT();
  processContextRequirements.needTempo();
  processContextRequirements.needContinousTimeSamples();
}

FBVST3Plugin::
~FBVST3Plugin()
{
  FB_LOG_ENTRY_EXIT();
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
  return kResultTrue;
}
