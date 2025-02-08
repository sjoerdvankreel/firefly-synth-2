#include <playground_base/base/topo/static/FBParamType.hpp>
#include <cassert>

bool
FBParamTypeIsStepped(FBParamType type)
{
  switch (type)
  {
  case FBParamType::List:
  case FBParamType::Boolean:
  case FBParamType::Discrete:
    return true;
  case FBParamType::FreqOct:
  case FBParamType::Linear:
    return false;
  default:
    assert(false);
    return false;
  }
}