#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <cassert>

using namespace juce;

IFBHorizontalAutoSize*
FBAsHorizontalAutoSize(Component* component)
{
  auto result = dynamic_cast<IFBHorizontalAutoSize*>(component);
  assert(result != nullptr);
  return result;
}