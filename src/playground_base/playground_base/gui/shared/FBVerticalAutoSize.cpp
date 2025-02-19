#include <playground_base/gui/shared/FBVerticalAutoSize.hpp>
#include <cassert>

using namespace juce;

IFBVerticalAutoSize*
FBAsVerticalAutoSize(Component* component)
{
  auto result = dynamic_cast<IFBVerticalAutoSize*>(component);
  assert(result != nullptr);
  return result;
}