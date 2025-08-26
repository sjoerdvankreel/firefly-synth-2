#include <firefly_base/gui/components/FBFillerComponent.hpp>

FBFillerComponent::
FBFillerComponent(int w, int h):
_w(w), _h(h) {}

int 
FBFillerComponent::FixedHeight() const
{
  return _h;
}

int 
FBFillerComponent::FixedWidth(int /*height*/) const
{
  return _w;
}
