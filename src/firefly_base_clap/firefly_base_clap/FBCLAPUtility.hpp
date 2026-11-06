#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <string>

struct FBStaticParam;

double
FBCLAPToNormalized(FBStaticParam const& topo, double clap);
double 
FBNormalizedToCLAP(FBStaticParam const& topo, double normalized);