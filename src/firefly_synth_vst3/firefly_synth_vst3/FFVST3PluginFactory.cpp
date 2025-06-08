#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/plug/FFPlugProcessor.hpp>

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <firefly_base_vst3/FBVST3AudioEffect.hpp>
#include <firefly_base_vst3/FBVST3EditController.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
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
  FFVST3AudioEffect(FBStaticTopo const& topo, FUID const& controllerId):
  FBVST3AudioEffect(topo, controllerId) {}

  std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() override
  { return std::make_unique<FFPlugProcessor>(this); }
};

bool
DeinitModule()
{
  FBGUITerminate();
  FBLogTerminate();
  return true;
}

bool 
InitModule()
{
  FBLogInit(FFPlugMeta(FBPlugFormat::VST3));
  FBGUIInit();
  return true;
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
  auto topo = FFMakeTopo(FBPlugFormat::VST3);
  auto result = new FBVST3EditController(*topo);
  return static_cast<IEditController*>(result);
}

static FUnknown*
ComponentFactory(void*)
{
  auto topo = FFMakeTopo(FBPlugFormat::VST3);
  auto controllerFuid = TextToFUID(FFPlugControllerId);
  auto result = new FFVST3AudioEffect(*topo, controllerFuid);
  return static_cast<IAudioProcessor*>(result);
}

BEGIN_FACTORY_DEF(FFVendorName, FFVendorURL, FFVendorMail)
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FFPlugUniqueId)),
      PClassInfo::kManyInstances, kVstAudioEffectClass, 
    FFPlugName, kDistributable, PlugType::kInstrument,
      FF_PLUG_VERSION, kVstVersionString, ComponentFactory);
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FFPlugControllerId)),
      PClassInfo::kManyInstances, kVstComponentControllerClass, 
      FFPlugName, 0, "",
      FF_PLUG_VERSION, kVstVersionString, ControllerFactory)
END_FACTORY