#pragma once

struct FBStaticParam;

double
FBCLAPToNormalized(FBStaticParam const& topo, double clap);
double 
FBNormalizedToCLAP(FBStaticParam const& topo, float normalized);