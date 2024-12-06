#include <playground_base/base/shared/FBLifetime.hpp>
#include <clap/helpers/plugin.hh>

using namespace clap;
using namespace clap::helpers;

class FBCLAPPlugin:
public Plugin<MisbehaviourHandler::Ignore, CheckingLevel::Maximal>
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBCLAPPlugin);
  FBCLAPPlugin(clap_plugin_descriptor const* desc, clap_host const* host);

  bool isValidParamId(clap_id paramId) const noexcept override;
  int32_t getParamIndexForParamId(clap_id paramId) const noexcept override;
  bool getParamInfoForParamId(clap_id paramId, clap_param_info* info) const noexcept override;
};