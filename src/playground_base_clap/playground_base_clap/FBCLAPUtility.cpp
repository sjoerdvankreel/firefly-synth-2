#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <cmath>

double
FBNormalizedToCLAP(FBStaticParam const& topo, float normalized)
{
  if (topo.ValueCount() == 0)
    return normalized;
  return std::round(normalized * (topo.ValueCount() - 1.0));
}

float
FBCLAPToNormalized(FBStaticParam const& topo, double clap)
{
  if (topo.ValueCount() == 0)
    return (float)clap;
  double normalized = clap / (topo.ValueCount() - 1.0);
  return (float)std::clamp(normalized, 0.0, 1.0);
}