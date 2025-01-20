#pragma once

#include <vector>
#include <functional>

typedef std::function<bool(
  std::vector<int> const& vs)>
FBDependencySelector;