#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>

using namespace juce;

static bool
IsTweakableParam(FBRuntimeTopo const* topo, int index)
{
  if (topo->audio.params[index].static_.output)
    return false;
  if (topo->audio.params[index].static_.thisIsNotARealParameter)
    return false;
  return true;
}

static std::string
GetParamTweakName(FBRuntimeParam const* param)
{
  if (param->static_.matrixName.size())
    return param->static_.matrixName;
  return param->displayName;
}

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
  setJustificationType(Justification::centred);
  plugGUI->AddParamListener(this);

  auto const* topo = plugGUI->HostContext()->Topo();
  for (int i = 0; i < topo->audio.params.size(); i++)
    if (IsTweakableParam(topo, i))
      _maxWidth = std::max(_maxWidth, 
        FBGUIGetStringWidthCached(GetParamTweakName(&topo->audio.params[i])));
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
FBLastTweakedLabel::AudioParamChangedFromUI(int index, double /*normalized*/)
{
  auto const* topo = _plugGUI->HostContext()->Topo();
  auto const& param = topo->audio.params[index];
  auto const& module = topo->modules[param.runtimeModuleIndex];
  auto const& staticModule = topo->static_->modules[module.topoIndices.index];
  auto const& tweakName = GetParamTweakName(&param);
  std::string const& moduleDisplay = staticModule.matrixName.size() ? staticModule.matrixName : module.name;
  std::string fullDisplay = moduleDisplay + " " + tweakName;
  if(FBGUIGetStringWidthCached(fullDisplay) <= _maxWidth)
    setText(fullDisplay, dontSendNotification);
  else
    setText(tweakName, dontSendNotification);
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
  if (index < 0 || !IsTweakableParam(_plugGUI->HostContext()->Topo(), index))
    return;
  auto const& param = _plugGUI->HostContext()->Topo()->audio.params[index];
  setText(param.NormalizedToText(false, normalized));
}