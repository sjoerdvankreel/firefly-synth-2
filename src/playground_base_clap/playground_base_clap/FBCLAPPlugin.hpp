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
};