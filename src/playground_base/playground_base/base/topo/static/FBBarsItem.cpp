#include <playground_base/base/topo/static/FBBarsItem.hpp>

std::string 
FBBarsItem::ToString() const
{
  if (num == 0)
    return "Off";
  return std::to_string(num) + "/" + std::to_string(denom);
}

bool
FBBarsItem::operator<(FBBarsItem const& rhs) const
{
  return num / (double)denom < rhs.num / (double)rhs.denom;
};

bool 
FBBarsItem::operator<=(FBBarsItem const& rhs) const
{
  return num / (double)denom <= rhs.num / (double)rhs.denom;
};