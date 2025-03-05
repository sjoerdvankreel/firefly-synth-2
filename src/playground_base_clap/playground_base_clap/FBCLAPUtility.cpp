#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>

#include <cmath>

double
FBNormalizedToCLAP(FBStaticParam const& topo, double normalized)
{
  if (!topo.NonRealTime().IsStepped())
    return normalized;
  return std::round(normalized * (topo.NonRealTime().ValueCount() - 1.0));
}

double
FBCLAPToNormalized(FBStaticParam const& topo, double clap)
{
  if (!topo.NonRealTime().IsStepped())
    return clap;
  return std::clamp(clap / (topo.NonRealTime().ValueCount() - 1.0), 0.0, 1.0);
}