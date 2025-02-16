#pragma once

struct FBStaticAudioParam;

double
FBCLAPToNormalized(FBStaticAudioParam const& topo, double clap);
double 
FBNormalizedToCLAP(FBStaticAudioParam const& topo, float normalized);