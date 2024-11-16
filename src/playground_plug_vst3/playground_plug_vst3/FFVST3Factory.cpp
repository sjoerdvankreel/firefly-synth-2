#include <playground_plug_vst3/FFVST3Processor.hpp>
#include <playground_plug_vst3/FFVST3Controller.hpp>
#include <playground_plug/plug/shared/FFConfiguration.hpp>

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
  return TextToFUID(FF_PLUGIN_CONTROLLER_ID);
}

static FUID
ProcessorFUID()
{
  return TextToFUID(FF_PLUGIN_PROCESSOR_ID);
}

static FUnknown*
ControllerFactory(void*) 
{
  auto result = new FFVST3Controller;
  return static_cast<IEditController*>(result);
}

static FUnknown*
ProcessorFactory(void*)
{
  auto result = new FFVST3Processor(ControllerFUID());
  return static_cast<IAudioProcessor*>(result);
}

BEGIN_FACTORY_DEF(FF_VENDOR_NAME, FF_VENDOR_URL, FF_VENDOR_MAIL)
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(ProcessorFUID()),
      PClassInfo::kManyInstances, kVstAudioEffectClass, 
      FF_PLUGIN_NAME, kDistributable, PlugType::kInstrument,
      FF_PLUGIN_VERSION, kVstVersionString, ProcessorFactory);
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(ControllerFUID()),
      PClassInfo::kManyInstances, kVstComponentControllerClass, 
      FF_PLUGIN_NAME, 0, "",
      FF_PLUGIN_VERSION, kVstVersionString, ControllerFactory)
END_FACTORY