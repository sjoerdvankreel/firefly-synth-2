#include <playground_base/base/shared/FBFormat.hpp>
#include <sstream>

std::string
FBFormatFloat(float val, int precision)
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(precision);
  ss << val;
  return ss.str();
}