#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>

using namespace juce;

FBLastTweakedLabel::
~FBLastTweakedLabel()
{
  _plugGUI->RemoveParamListener(this);
}

FBLastTweakedLabel::
FBLastTweakedLabel(FBPlugGUI* plugGUI) :
_plugGUI(plugGUI)
{
  setText("Tweak", dontSendNotification);
  plugGUI->AddParamListener(this);
  _maxWidth = 80;
}

int
FBLastTweakedLabel::FixedHeight() const
{
  return 20;
}

int
FBLastTweakedLabel::FixedWidth(int /*height*/) const
{
  return getBorderSize().getLeftAndRight() + _maxWidth;
}

void 
FBLastTweakedLabel::AudioParamChangedFromUI(int index, double normalized)
{
  auto const& param = _plugGUI->HostContext()->Topo()->audio.params[index];
  setText(param.displayName, dontSendNotification);
}

FBLastTweakedTextBox::
~FBLastTweakedTextBox()
{
  _plugGUI->RemoveParamListener(this);
}

FBLastTweakedTextBox::
FBLastTweakedTextBox(FBPlugGUI* plugGUI, int fixedWidth):
_fixedWidth(fixedWidth), _plugGUI(plugGUI)
{
  setText("Knob...", dontSendNotification);
  plugGUI->AddParamListener(this);
}

int
FBLastTweakedTextBox::FixedHeight() const
{
  return 24;
}

int
FBLastTweakedTextBox::FixedWidth(int /*height*/) const
{
  return _fixedWidth;
}

void 
FBLastTweakedTextBox::AudioParamChangedFromUI(int index, double normalized)
{
  _paramIndex = index;
  if (index < 0)
    return;
  auto const& param = _plugGUI->HostContext()->Topo()->audio.params[index];
  setText(param.NormalizedToText(false, normalized));
}