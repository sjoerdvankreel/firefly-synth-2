#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/base/ibstream.h>

#include <string>

using namespace Steinberg;
using namespace Steinberg::Vst;

std::string 
FBVST3LoadIBStream(IBStream* stream);
void
FBVST3CopyToString128(std::string const& in, TChar* out);