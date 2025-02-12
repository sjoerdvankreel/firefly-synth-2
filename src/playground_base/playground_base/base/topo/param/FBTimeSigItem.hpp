#pragma once

#include <string>

struct FBTimeSigItem final
{
  int num = 0;
  int denom = -1;
  std::string ToString() const;
};