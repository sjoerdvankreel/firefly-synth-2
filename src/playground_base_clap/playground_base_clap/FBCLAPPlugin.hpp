#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/host/FBHostOutputBlock.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessContext.hpp>
#include <playground_base_clap/FBCLAPSyncEvent.hpp>

#include <juce_events/juce_events.h>
#include <clap/helpers/plugin.hh>
#include <readerwriterqueue.h>
#include <memory>

using namespace clap;
using namespace clap::helpers;

struct FBStaticTopo;
struct FBRuntimeTopo;

class IFBPlugProcessor;
class FBPlugGUIContext;

class FBCLAPPlugin:
public Plugin<MisbehaviourHandler::Ignore, CheckingLevel::Maximal>,
public juce::Timer,
public IFBHostProcessContext,
public IFBHostGUIContext
{
  std::unique_ptr<FBRuntimeTopo> _topo;
  FBProcStateContainer _procState;
  FBScalarStateContainer _editState;
  std::unique_ptr<FBPlugGUIContext> _gui;

  moodycamel::ReaderWriterQueue<FBCLAPSyncToMainEvent, 
    FBCLAPSyncEventReserve> _audioToMainEvents;
  moodycamel::ReaderWriterQueue<FBCLAPSyncToAudioEvent, 
    FBCLAPSyncEventReserve> _mainToAudioEvents;

  FBHostInputBlock _input = {};
  FBHostOutputBlock _output = {};
  std::array<std::vector<float>, 2> _zeroIn = {};
  std::unique_ptr<FBHostProcessor> _hostProcessor = {};

  void PushParamChangeToProcessorBlock(int index, double normalized, int pos);
  void ProcessMainToAudioEvents(const clap_output_events* out, bool pushToProcBlock);

protected:
  virtual std::unique_ptr<IFBPlugProcessor>
  MakePlugProcessor(FBStaticTopo const& topo, void* rawState, float sampleRate) const = 0;

public:
  ~FBCLAPPlugin();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBCLAPPlugin);
  FBCLAPPlugin(FBStaticTopo const& topo, clap_plugin_descriptor const* desc, clap_host const* host);

  bool init() noexcept override;
  void timerCallback() override;

  void ProcessVoices() override;
  void EndParamChange(int index) override;
  void BeginParamChange(int index) override;
  float GetParamNormalized(int index) const override;
  void PerformParamEdit(int index, float normalized) override;

  uint32_t latencyGet() const noexcept override;
  bool implementsLatency() const noexcept override;

  bool implementsThreadPool() const noexcept override;
  void threadPoolExec(uint32_t taskIndex) noexcept override;

  bool implementsState() const noexcept override;
  bool stateSave(const clap_ostream* stream) noexcept override;
  bool stateLoad(const clap_istream* stream) noexcept override;

  bool guiShow() noexcept override;
  bool guiHide() noexcept override;
  void guiDestroy() noexcept override;
  bool guiCanResize() const noexcept override;
  bool implementsGui() const noexcept override;
  bool guiSetScale(double scale) noexcept override;
  bool guiSetParent(const clap_window* window) noexcept override;
  bool guiSetSize(uint32_t width, uint32_t height) noexcept override;
  bool guiCreate(const char* api, bool isFloating) noexcept override;
  bool guiGetSize(uint32_t* width, uint32_t* height) noexcept override;
  bool guiAdjustSize(uint32_t* width, uint32_t* height) noexcept override;
  bool guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept override;
  bool guiIsApiSupported(const char* api, bool isFloating) noexcept override;

  bool isValidParamId(clap_id paramId) const noexcept override;
  int32_t getParamIndexForParamId(clap_id paramId) const noexcept override;
  bool getParamInfoForParamId(clap_id paramId, clap_param_info* info) const noexcept override;

  clap_process_status process(const clap_process* process) noexcept override;
  bool activate(double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept override;

  uint32_t paramsCount() const noexcept override;
  bool implementsParams() const noexcept override;
  bool paramsValue(clap_id paramId, double* value) noexcept override;
  bool paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept override;
  bool paramsTextToValue(clap_id paramId, const char* display, double* value) noexcept override;
  void paramsFlush(const clap_input_events* in, const clap_output_events* out) noexcept override;
  bool paramsValueToText(clap_id paramId, double value, char* display, uint32_t size) noexcept override;

  bool implementsNotePorts() const noexcept override;
  uint32_t notePortsCount(bool isInput) const noexcept override;
  bool notePortsInfo(uint32_t index, bool isInput, clap_note_port_info* info) const noexcept override;

  bool implementsAudioPorts() const noexcept override;
  uint32_t audioPortsCount(bool isInput) const noexcept override;
  bool audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info* info) const noexcept override;
};