#include <playground_plug_vst3/FFVST3PluginProcessor.hpp>
#include <playground_plug_vst3/FFVST3PluginController.hpp>
#include <playground_plug/shared/FFDefines.hpp>
#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

static FUID
TextToFUID(char const* text)
{
  FUID result;
  result.fromString(text);
  return result;
}

static FUID
ControllerFUID()
{
  return TextToFUID(FF_SYNTH_CONTROLLER_ID);
}

static FUID
ProcessorFUID()
{
  return TextToFUID(FF_SYNTH_PROCESSOR_ID);
}

static FUnknown*
ControllerFactory(void*) 
{
  auto result = new FFVST3PluginController;
  return static_cast<IEditController*>(result);
}

static FUnknown*
ProcessorFactory(void*)
{
  auto result = new FFVST3PluginProcessor(ControllerFUID());
  return static_cast<IAudioProcessor*>(result);
}

BEGIN_FACTORY_DEF(FF_VENDOR_NAME, FF_VENDOR_URL, FF_VENDOR_MAIL)
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(ProcessorFUID()),
      PClassInfo::kManyInstances, kVstAudioEffectClass, 
      FF_SYNTH_NAME, kDistributable, PlugType::kInstrument, 
      FF_VERSION, kVstVersionString, ProcessorFactory);
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(ControllerFUID()),
      PClassInfo::kManyInstances, kVstComponentControllerClass, 
      FF_SYNTH_NAME, 0, "", 
      FF_VERSION, kVstVersionString, ControllerFactory)
END_FACTORY