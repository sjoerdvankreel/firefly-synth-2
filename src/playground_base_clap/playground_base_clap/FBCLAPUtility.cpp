#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>

#include <cmath>

double
FBNormalizedToCLAP(FBStaticParam const& topo, float normalized)
{
  if (topo.NonRealTime().ValueCount() == 0)
    return normalized;
  return std::round(normalized * (topo.NonRealTime().ValueCount() - 1.0));
}

double
FBCLAPToNormalized(FBStaticParam const& topo, double clap)
{
  if (topo.NonRealTime().ValueCount() == 0)
    return clap;
  double normalized = clap / (topo.NonRealTime().ValueCount() - 1.0);
  return std::clamp(normalized, 0.0, 1.0);
}