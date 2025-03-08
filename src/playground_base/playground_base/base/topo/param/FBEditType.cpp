#include <playground_base/base/topo/param/FBEditType.hpp>
#include <cassert>

std::string
FBEditTypeToString(FBEditType type)
{
  switch (type)
  {
  case FBEditType::Linear: return "Linear";
  case FBEditType::Stepped: return "Stepped";
  case FBEditType::Logarithmic: return "Logarithmic";
  default: assert(false); return {};
  }
}