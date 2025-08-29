#include <firefly_base/gui/controls/FBTextBox.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

using namespace juce;

FBAutoSizeTextBox::
FBAutoSizeTextBox(int fixedWidth):
_fixedWidth(fixedWidth) {}

int 
FBAutoSizeTextBox::FixedHeight() const
{
  return 24;
}

int
FBAutoSizeTextBox::FixedWidth(int /*height*/) const
{
  return _fixedWidth;
}

FBLastTweakedTextBox::
~FBLastTweakedTextBox()
{
  _plugGUI->RemoveParamListener(this);
}

FBLastTweakedTextBox::
FBLastTweakedTextBox(FBPlugGUI* plugGUI, int fixedWidth):
FBAutoSizeTextBox(fixedWidth),
_plugGUI(plugGUI)
{
  setText("Knob...", dontSendNotification);
  plugGUI->AddParamListener(this);
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