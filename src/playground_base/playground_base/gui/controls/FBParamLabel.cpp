#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>

#include <cmath>
#include <string>

using namespace juce;

FBParamLabel::
FBParamLabel(FBRuntimeParam const* param):
Label()
{
  setText(param->shortName, dontSendNotification);
}

int 
FBParamLabel::FixedWidth(int height) const
{
  auto border = getBorderSize().getLeftAndRight();
  return border + (int)std::ceil(TextLayout::getStringWidth(getFont(), getText()));
}