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
  case FBParamType::Boolean:
  case FBParamType::Discrete:
  case FBParamType::DiscreteLog2:
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
  case FBParamType::DiscreteLog2:
    return true;
  case FBParamType::Log2:
  case FBParamType::Linear:
    return false;
  default:
    assert(false);
    return false;
  }
}