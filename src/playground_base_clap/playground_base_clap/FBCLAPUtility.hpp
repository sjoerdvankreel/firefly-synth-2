#pragma once

struct FBStaticParam;

float
FBCLAPToNormalized(FBStaticParam const& topo, double clap);
double 
FBNormalizedToCLAP(FBStaticParam const& topo, float normalized);