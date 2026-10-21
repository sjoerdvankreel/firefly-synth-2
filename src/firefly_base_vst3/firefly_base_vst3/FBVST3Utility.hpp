#pragma once

#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/base/ibstream.h>

#include <vector>
#include <string>
#include <limits>
#include <cstdint>

using namespace Steinberg;
using namespace Steinberg::Vst;

inline std::string const FBVST3LoadAllAudioClipsMessageId = "FBVST3LoadAllAudioClipsMessageId";
inline std::string const FBVST3LoadSingleAudioClipMessageId = "FBVST3LoadSingleAudioClipMessageId";
inline std::string const FBVST3ClearSingleAudioClipMessageId = "FBVST3ClearSingleAudioClipMessageId";

inline int constexpr FBVST3MIDIMaxChannels = 16;
inline int constexpr FBVST3MIDIMaxPortsGuess = 128;
inline int constexpr FBVST3MIDIMaxMessages = FBMIDIEvent::MessageCount;
inline int constexpr FBVST3MIDIMaxMappableParamsGuess = FBVST3MIDIMaxPortsGuess * FBVST3MIDIMaxChannels * FBVST3MIDIMaxMessages;
inline int constexpr FBVST3MIDIParameterMappingBegin = std::numeric_limits<int>::max() - FBVST3MIDIMaxMappableParamsGuess;

// Duplicated between vst3 and clap.
// VST3 goes thru the host queue first, is received by the processor (on the gui thread no less),
// then pushed from processor "gui" to processor realtime.
// CLAP skips the host queue (IMessage).
// TODO Sampler
struct FBVST3AudioClipToDSPCommand
{
  enum { ClearSingle, LoadSingle, LoadAll } Type;
  std::string id = {};
  FBAudioClip clip = {};
  std::map<std::string, FBAudioClip> allClips = {};
};

void
FBVST3CopyToString128(std::string const& in, TChar* out);
void
FBVST3CopyFromString128(TChar const* in, std::string& out);

bool
FBVST3LoadIBStream(IBStream* stream, std::string& state);
bool
FBVST3SaveIBStream(IBStream* stream, std::string const& state);