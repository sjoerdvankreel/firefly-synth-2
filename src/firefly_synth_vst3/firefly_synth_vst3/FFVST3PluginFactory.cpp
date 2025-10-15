#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/plug/FFPlugProcessor.hpp>

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/dsp/shared/FBTuning.hpp>
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

#ifdef FF_IS_FX
#if FF_IS_FX
auto constexpr FFPlugType = PlugType::kFx;
auto constexpr FFPlugName = FFPlugNameFX;
auto constexpr FFPlugUniqueId = FFPlugUniqueIdFX;
auto constexpr FFPlugControllerId = FFPlugControllerIdFX;
#else
auto constexpr FFPlugType = PlugType::kInstrument;
auto constexpr FFPlugName = FFPlugNameInst;
auto constexpr FFPlugUniqueId = FFPlugUniqueIdInst;
auto constexpr FFPlugControllerId = FFPlugControllerIdInst;
#endif
#else
#error
#endif

class MTSClient;

class FFVST3AudioEffect:
public FBVST3AudioEffect
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVST3AudioEffect);
  FFVST3AudioEffect(std::unique_ptr<FFStaticTopo>&& topo, FUID const& controllerId, MTSClient* mtsClient):
  FBVST3AudioEffect(std::move(topo), controllerId, mtsClient) {}

  std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() override
  { return std::make_unique<FFPlugProcessor>(this); }
};

bool
DeinitModule()
{
  FBTuningTerminate();
  FBGUITerminate();
  FBLogTerminate();
  return true;
}

bool 
InitModule()
{
  FBLogInit(FFPlugMeta(FBPlugFormat::VST3, FF_IS_FX != 0));
  FBGUIInit();
  FBTuningInit();
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
  return FBWithLogException([]() 
  {
    auto result = new FBVST3EditController(FFMakeTopo(FBPlugFormat::VST3, FF_IS_FX != 0));
    return static_cast<IEditController*>(result);
  });
}

static FUnknown*
ComponentFactory(void*)
{
  return FBWithLogException([]() 
  {
    auto controllerFuid = TextToFUID(FFPlugControllerId);
    auto result = new FFVST3AudioEffect(FFMakeTopo(FBPlugFormat::VST3, FF_IS_FX != 0), controllerFuid, FBTuningGetMTSClient());
    return static_cast<IAudioProcessor*>(result);
  });
}

BEGIN_FACTORY_DEF(FFVendorName, FFVendorURL, FFVendorMail)
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FFPlugUniqueId)),
      PClassInfo::kManyInstances, kVstAudioEffectClass, 
      FFPlugName, kDistributable, FFPlugType,
      FF_PLUG_VERSION, kVstVersionString, ComponentFactory);
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FFPlugControllerId)),
      PClassInfo::kManyInstances, kVstComponentControllerClass, 
      FFPlugName, 0, "",
      FF_PLUG_VERSION, kVstVersionString, ControllerFactory)
END_FACTORY