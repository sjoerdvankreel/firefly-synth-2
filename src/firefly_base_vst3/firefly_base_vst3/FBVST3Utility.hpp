#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/base/ibstream.h>

#include <string>
#include <limits>
#include <cstdint>

using namespace Steinberg;
using namespace Steinberg::Vst;

inline int constexpr FBVST3MIDIMaxChannels = 16;
inline int constexpr FBVST3MIDIMaxPortsGuess = 128;
inline int constexpr FBVST3MIDIMaxEventIds = FBMIDIEvent::EventCount;
inline int constexpr FBVST3MIDIMaxMappableParamsGuess = FBVST3MIDIMaxPortsGuess * FBVST3MIDIMaxChannels * FBVST3MIDIMaxEventIds;
inline int constexpr FBVST3MIDIParameterMappingBegin = std::numeric_limits<int>::max() - FBVST3MIDIMaxMappableParamsGuess;

void
FBVST3CopyToString128(std::string const& in, TChar* out);
void
FBVST3CopyFromString128(TChar const* in, std::string& out);

bool
FBVST3LoadIBStream(IBStream* stream, std::string& state);
bool
FBVST3SaveIBStream(IBStream* stream, std::string const& state);