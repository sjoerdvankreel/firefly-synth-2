#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>

#include <clap/helpers/plugin.hh>
#include <memory>

using namespace clap;
using namespace clap::helpers;

struct FBStaticTopo;
struct FBRuntimeTopo;

class FBCLAPPlugin:
public Plugin<MisbehaviourHandler::Ignore, CheckingLevel::Maximal>
{
  std::unique_ptr<FBRuntimeTopo> _topo;
  void* _procState;
  FBProcStatePtrs _procStatePtrs;

public:
  ~FBCLAPPlugin();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBCLAPPlugin);
  FBCLAPPlugin(FBStaticTopo const& topo, clap_plugin_descriptor const* desc, clap_host const* host);

  uint32_t latencyGet() const noexcept override;
  bool implementsLatency() const noexcept override;

  bool implementsState() const noexcept override;
  bool stateSave(const clap_ostream* stream) noexcept override;
  bool stateLoad(const clap_istream* stream) noexcept override;

  bool isValidParamId(clap_id paramId) const noexcept override;
  int32_t getParamIndexForParamId(clap_id paramId) const noexcept override;
  bool getParamInfoForParamId(clap_id paramId, clap_param_info* info) const noexcept override;

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