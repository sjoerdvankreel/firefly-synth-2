#include <playground_plug/shared/FFPluginTopo.hpp>
#include <playground_plug/shared/FFPluginConfig.hpp>
#include <playground_plug/dsp/FFPluginProcessor.hpp>

#include <playground_base_vst3/FBVST3AudioEffect.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>

#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3AudioEffect:
public FBVST3AudioEffect
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVST3AudioEffect);
  FFVST3AudioEffect(FBRuntimeTopo&& topo, FUID const& controllerId);

protected:
  std::unique_ptr<IFBHostBlockProcessor> CreateProcessor(
    FBRuntimeTopo const& topo, ProcessSetup const& setup) const override;
};

FFVST3AudioEffect::
FFVST3AudioEffect(FBRuntimeTopo&& topo, FUID const& controllerId) :
FBVST3AudioEffect(std::move(topo), controllerId) {}

std::unique_ptr<IFBHostBlockProcessor>
FFVST3AudioEffect::CreateProcessor(
  FBRuntimeTopo const& topo, ProcessSetup const& setup) const
{
  return std::make_unique<FFPluginProcessor>(topo, setup.maxSamplesPerBlock, setup.sampleRate);
}

static FUID
TextToFUID(char const* text)
{
  FUID result;
  result.fromString(text);
  return result;
}

static FUnknown*
ControllerFactory(void*) 
{
  auto result = new FBVST3EditController(FFMakeTopo());
  return static_cast<IEditController*>(result);
}

static FUnknown*
ComponentFactory(void*)
{
  auto controllerFuid = TextToFUID(FF_PLUGIN_CONTROLLER_ID);
  auto result = new FFVST3AudioEffect(FFMakeTopo(), controllerFuid);
  return static_cast<IAudioProcessor*>(result);
}

BEGIN_FACTORY_DEF(FF_VENDOR_NAME, FF_VENDOR_URL, FF_VENDOR_MAIL)
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FF_PLUGIN_PROCESSOR_ID)),
      PClassInfo::kManyInstances, kVstAudioEffectClass, 
      FF_PLUGIN_NAME, kDistributable, PlugType::kInstrument,
      FF_PLUGIN_VERSION, kVstVersionString, ComponentFactory);
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FF_PLUGIN_CONTROLLER_ID)),
      PClassInfo::kManyInstances, kVstComponentControllerClass, 
      FF_PLUGIN_NAME, 0, "",
      FF_PLUGIN_VERSION, kVstVersionString, ControllerFactory)
END_FACTORY