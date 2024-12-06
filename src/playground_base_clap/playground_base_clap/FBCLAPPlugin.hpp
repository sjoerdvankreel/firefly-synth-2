#include <playground_base/base/shared/FBLifetime.hpp>

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

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBCLAPPlugin);
  FBCLAPPlugin(FBStaticTopo const& topo, clap_plugin_descriptor const* desc, clap_host const* host);

  bool isValidParamId(clap_id paramId) const noexcept override;
  int32_t getParamIndexForParamId(clap_id paramId) const noexcept override;
  bool getParamInfoForParamId(clap_id paramId, clap_param_info* info) const noexcept override;

  uint32_t paramsCount() const noexcept override;
  bool implementsParams() const noexcept override { return true; }
  bool paramsValue(clap_id paramId, double* value) noexcept override;
  bool paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept override;
  bool paramsTextToValue(clap_id paramId, const char* display, double* value) noexcept override;
  void paramsFlush(const clap_input_events* in, const clap_output_events* out) noexcept override;
  bool paramsValueToText(clap_id paramId, double value, char* display, uint32_t size) noexcept override;
};