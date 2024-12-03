#include <playground_base/base/topo/FBStaticParam.hpp>
#include <cassert>

std::string
FBStaticParam::NormalizedToText(bool io, double normalized) const
{
  assert(valueCount != 1);
  if (valueCount == 0)
    return std::to_string(normalized);
  int discrete = NormalizedToDiscrete(normalized);
  if (list.size() != 0)
    return io ? list[discrete].id : list[discrete].text;
  if (valueCount == 2)
    return discrete == 0 ? "Off" : "On";
  return std::to_string(discrete);
}