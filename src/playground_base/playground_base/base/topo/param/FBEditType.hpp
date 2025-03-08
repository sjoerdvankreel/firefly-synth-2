#pragma once

#include <string>

enum class FBEditType
{
  Linear,
  Stepped,
  Logarithmic
};

std::string
FBEditTypeToString(FBEditType type);