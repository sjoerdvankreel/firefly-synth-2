#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugMeta.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFPlugProcessor.hpp>

#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base_clap/FBCLAPExchangeStateQueue.hpp>

#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <clap/clap.h>

#include <cassert>
#include <cstring>

// Just to make it work with the moodycamel queue without
// introducing accidental copy/move support into the plugin itself.
struct alignas(alignof(FFExchangeState)) FFCLAPExchangeState final
{
  FFExchangeState state;
  FFCLAPExchangeState(FFCLAPExchangeState const& rhs);
  FFCLAPExchangeState& operator=(FFCLAPExchangeState const& rhs);
};

FFCLAPExchangeState::
FFCLAPExchangeState(FFCLAPExchangeState const& rhs)
{
  assert(&rhs != this);
  memcpy(&state, &rhs.state, sizeof(FFExchangeState));
}

FFCLAPExchangeState&
FFCLAPExchangeState::operator=(FFCLAPExchangeState const& rhs)
{
  assert(&rhs != this);
  memcpy(&state, &rhs.state, sizeof(FFExchangeState));
  return *this;
}

class FFCLAPPlugin:
public FBCLAPPlugin
{
public:
  std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() override
  { return std::make_unique<FFPlugProcessor>(this); }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFCLAPPlugin);
  FFCLAPPlugin(FBStaticTopo const& topo, clap_plugin_descriptor const* desc, clap_host const* host):
  FBCLAPPlugin(topo, desc, host, std::make_unique<FBCLAPExchangeStateQueue<FFCLAPExchangeState>>()) {}
};

static void CLAP_ABI 
Deinit()
{
  juce::shutdownJuce_GUI();
  FBLogTerminate();
}

static bool CLAP_ABI 
Init(char const*) 
{ 
  FBLogInit(FFPlugMeta());
  juce::initialiseJuce_GUI();
  return true; 
}

static char const*
Features[] = { CLAP_PLUGIN_FEATURE_INSTRUMENT, nullptr };
static std::uint32_t CLAP_ABI
GetPluginCount(struct clap_plugin_factory const*) { return 1; }

static clap_plugin_descriptor_t const*
GetPluginDescriptor(
  clap_plugin_factory const*, std::uint32_t)
{
  static clap_plugin_descriptor_t result = {};
  result.features = Features;
  result.url = FFVendorURL;
  result.name = FFPlugName;
  result.id = FFPlugUniqueId;
  result.vendor = FFVendorName;
  result.version = FF_PLUG_VERSION;
  result.clap_version = CLAP_VERSION_INIT;
  return &result;
}; 

static clap_plugin_t const* CLAP_ABI
CreatePlugin(
  struct clap_plugin_factory const* factory,
  clap_host_t const* host, char const* pluginId)
{
  auto topo = FFMakeTopo();
  auto const* desc = GetPluginDescriptor(nullptr, 0);
  if (strcmp(desc->id, pluginId))
    return static_cast<clap_plugin_t const*>(nullptr);
  return (new FFCLAPPlugin(*topo, desc, host))->clapPlugin();
}

static void const*
GetFactory(char const* factoryId)
{
  if (strcmp(factoryId, CLAP_PLUGIN_FACTORY_ID)) 
    return nullptr;

  static clap_plugin_factory result;
  result.create_plugin = CreatePlugin;
  result.get_plugin_count = GetPluginCount;
  result.get_plugin_descriptor = GetPluginDescriptor;
  return &result;
}

extern "C" CLAP_EXPORT
clap_plugin_entry_t const clap_entry =
{
  .clap_version = CLAP_VERSION_INIT,
  .init = Init,
  .deinit = Deinit,
  .get_factory = GetFactory
};