#pragma once

#include <string>

struct FBBarsItem final
{
  int num = 0;
  int denom = -1;
  
  std::string ToString() const;  
  bool operator<(FBBarsItem const& rhs) const;
  bool operator<=(FBBarsItem const& rhs) const;
};