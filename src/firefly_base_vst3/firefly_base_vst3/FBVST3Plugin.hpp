#pragma once

#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/base/shared/FBHostContext.hpp>

#include <public.sdk/source/vst/vstsinglecomponenteffect.h>
#include <readerwriterqueue.h>

#include <array>
#include <vector>
#include <memory>
#include <thread>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;

class MTSClient;
class FBHostProcessor;
class IFBPlugProcessor;
class FBVST3GUIEditor;
class FBGUIStateContainer;
class FBProcStateContainer;
class FBExchangeStateQueueBase;
class FBExchangeStateContainer;

class FBVST3Plugin :
public SingleComponentEffect,
public IMidiMapping,
public FBHostContext
{
  // gui
  FBVST3GUIEditor* _guiEditor = {};

  // realtime
  float _sampleRate = 0.0f;
  FBHostInputBlock _input = {};
  FBHostOutputBlock _output = {};
  std::array<std::vector<float>, 2> _zeroIn = {};
  std::unique_ptr<FBHostProcessor> _hostProcessor = {};

  void InitializeAudioEffect();
  void InitializeEditController();

protected:

  FBVST3Plugin(
    std::unique_ptr<FBStaticTopo>&& topo,
    MTSClient* mtsClient,
    std::unique_ptr<FBExchangeStateQueueBase>&& exchangeStateQueue);

  // FBHostContext
  void OnReceivedExchangeFromDSP() override;
  void DoEndAudioParamChange(int index) override;
  void DoBeginAudioParamChange(int index) override;
  void DoPerformAudioParamEdit(int index, double normalized) override;

public:

#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4458)
#endif
  DEFINE_INTERFACES
    DEF_INTERFACE(IMidiMapping)
    DEF_INTERFACE(IEditController)
  END_DEFINE_INTERFACES(SingleComponentEffect)
  DELEGATE_REFCOUNT(SingleComponentEffect)
#if _MSC_VER
#pragma warning(pop)
#endif

  ~FBVST3Plugin();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3Plugin);

  // for gui editor
  void ResetView();

  // FBHostContext (gui)
  void NotifyHostOfParamNameChanges() override;
  void ClearAudioParamNameOverrides() override;
  void ClearAudioParamNameOverride(int index) override;
  bool GetAudioParamNameOverride(int index, std::string& name) const override;
  void SetAudioParamNameOverride(int index, std::string const& name) override;
  double GetAudioParamNormalized(int index) const override;
  void AudioParamContextMenuClicked(int paramIndex, int juceTag) override;
  std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) override;

  // FBHostContext (dsp)
  float SampleRate() const override { return _sampleRate; }

  // SingleComponentEffect, both
  tresult PLUGIN_API initialize(FUnknown* context) override;

  // SingleComponentEffect, controller
  tresult PLUGIN_API setEditorState(IBStream* state) override;
  tresult PLUGIN_API getEditorState(IBStream* state) override;
  IPlugView* PLUGIN_API createView(FIDString name) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;
  tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) override;
  tresult PLUGIN_API getMidiControllerAssignment(int32 bus, int16 channel, CtrlNumber number, ParamID& id) override;

  // SingleComponentEffect, processor
  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;
  tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
  uint32 PLUGIN_API getLatencySamples() override { return FBFixedBlockSamples; }
};