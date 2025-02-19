#include <playground_base/base/topo/static/FBParamType.hpp>
#include <cassert>

bool
FBParamTypeIsList(FBParamType type)
{
  switch (type)
  {
  case FBParamType::List:
  case FBParamType::Note:
  case FBParamType::Bars:
    return true;
  case FBParamType::Log2:
  case FBParamType::Linear:
  case FBParamType::FreqOct:
  case FBParamType::Boolean:
  case FBParamType::Discrete:
    return false;
  default:
    assert(false);
    return false;
  }
}

bool
FBParamTypeIsStepped(FBParamType type)
{
  switch (type)
  {
  case FBParamType::List:
  case FBParamType::Note:
  case FBParamType::Bars:
  case FBParamType::Boolean:
  case FBParamType::Discrete:
    return true;
  case FBParamType::Log2:
  case FBParamType::Linear:
  case FBParamType::FreqOct:
    return false;
  default:
    assert(false);
    return false;
  }
}