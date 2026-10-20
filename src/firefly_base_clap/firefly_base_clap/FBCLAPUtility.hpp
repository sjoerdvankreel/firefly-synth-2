#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <string>

struct FBStaticParam;

// Duplicated between vst3 and clap.
// VST3 goes thru the host queue first, is received by the processor (on the gui thread no less),
// then pushed from processor "gui" to processor realtime.
// CLAP skips the host queue (IMessage).
struct FBCLAPAudioClipToDSPCommand
{
  enum { ClearSingle, LoadSingle, LoadAll } Type;
  std::string id = {};
  FBAudioClip clip = {};
  std::map<std::string, FBAudioClip> allClips = {};
};

double
FBCLAPToNormalized(FBStaticParam const& topo, double clap);
double 
FBNormalizedToCLAP(FBStaticParam const& topo, double normalized);