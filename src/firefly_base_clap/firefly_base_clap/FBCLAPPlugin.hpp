#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/host/FBHostDSPContext.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base_clap/FBCLAPSyncEvent.hpp>

#include <juce_events/juce_events.h>
#include <clap/helpers/plugin.hh>
#include <readerwriterqueue.h>
#include <memory>

using namespace clap;
using namespace clap::helpers;

struct FBStaticTopo;
struct FBRuntimeTopo;

class FBHostProcessor;
class IFBPlugProcessor;
class FBPlugGUIContext;
class FBGUIStateContainer;
class FBProcStateContainer;
class FBScalarStateContainer;
class FBExchangeStateContainer;
class FBCLAPExchangeStateQueueBase;

class FBCLAPPlugin:
public Plugin<MisbehaviourHandler::Ignore, CheckingLevel::Maximal>,
public juce::Timer,
public FBHostGUIContext,
public IFBHostDSPContext
{
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4324)
#endif
  moodycamel::ReaderWriterQueue<FBCLAPSyncToMainEvent,
    FBCLAPSyncEventReserve> _audioToMainEvents;
  moodycamel::ReaderWriterQueue<FBCLAPSyncToAudioEvent,
    FBCLAPSyncEventReserve> _mainToAudioEvents;
#if _MSC_VER
#pragma warning(pop)
#endif

  std::unique_ptr<FBPlugGUIContext> _gui;
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::unique_ptr<FBGUIStateContainer> _guiState;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBScalarStateContainer> _editState;
  std::unique_ptr<FBExchangeStateContainer> _dspExchangeState;
  std::unique_ptr<FBExchangeStateContainer> _guiExchangeState;
  std::unique_ptr<FBCLAPExchangeStateQueueBase> _exchangeStateQueue;

  float _sampleRate = 0.0f;
  FBHostInputBlock _input = {};
  FBHostOutputBlock _output = {};
  std::array<std::vector<float>, 2> _zeroIn = {};
  std::unique_ptr<FBHostProcessor> _hostProcessor = {};

  std::unique_ptr<clap_context_menu_target> 
  MakeAudioParamContextMenu(int index, std::vector<FBHostContextMenuItem>& items);
  void PushParamChangeToProcessorBlock(int index, double normalized, int pos);
  void ProcessMainToAudioEvents(const clap_output_events* out, bool pushToProcBlock);

protected:
  void DoEndAudioParamChange(int index) override;
  void DoBeginAudioParamChange(int index) override;
  void DoPerformAudioParamEdit(int index, double normalized) override;

public:
  ~FBCLAPPlugin();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBCLAPPlugin);
  FBCLAPPlugin(
    FBStaticTopo const& topo,
    clap_plugin_descriptor const* desc, 
    clap_host const* host,    
    std::unique_ptr<FBCLAPExchangeStateQueueBase>&& exchangeStateQueue);

  bool init() noexcept override;
  void timerCallback() override;

  uint32_t latencyGet() const noexcept override;
  bool implementsLatency() const noexcept override;

  bool implementsState() const noexcept override;
  bool stateSave(const clap_ostream* stream) noexcept override;
  bool stateLoad(const clap_istream* stream) noexcept override;

#if (defined __linux__) || (defined  __FreeBSD__)
  bool implementsPosixFdSupport() const noexcept override;
  void onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept override;
#endif

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

  float SampleRate() const override { return _sampleRate; }
  FBRuntimeTopo const* Topo() const override { return _topo.get(); }
  FBProcStateContainer* ProcState() override { return _procState.get(); }
  FBExchangeStateContainer* ExchangeState() override { return _dspExchangeState.get(); }

  double GetAudioParamNormalized(int index) const override;
  double GetGUIParamNormalized(int index) const override;
  void SetGUIParamNormalized(int index, double normalized) override;
  
  void AudioParamContextMenuClicked(int paramIndex, int juceTag) override;
  std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) override;

  FBGUIStateContainer* GUIState() override { return _guiState.get(); }
  FBExchangeStateContainer const* ExchangeState() const override { return _guiExchangeState.get(); }

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