#include <playground_base/base/topo/param/FBBarsItem.hpp>

std::string 
FBBarsItem::ToString() const
{
  return std::to_string(num) + "/" + std::to_string(denom);
}