#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/base/ibstream.h>

#include <string>

using namespace Steinberg;
using namespace Steinberg::Vst;

void
FBVST3CopyToString128(std::string const& in, TChar* out);
void
FBVST3CopyFromString128(TChar const* in, std::string& out);

bool
FBVST3LoadIBStream(IBStream* stream, std::string& state);
bool
FBVST3SaveIBStream(IBStream* stream, std::string const& state);