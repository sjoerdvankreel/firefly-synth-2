#include <playground_base/base/topo/param/FBTimeSigItem.hpp>

std::string 
FBTimeSigItem::ToString() const
{
  return std::to_string(num) + "/" + std::to_string(denom);
}