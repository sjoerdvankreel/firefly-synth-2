#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static bool
IsTweakableParam(FBRuntimeTopo const* topo, int index)
{
  switch (topo->audio.params[index].static_.mode)
  {
  case FBParamMode::Fake: return false;
  case FBParamMode::Output: return false;
  default: return true;
  }
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
  plugGUI->AddParamListener(this);

  auto const* topo = plugGUI->HostContext()->Topo();
  for (int i = 0; i < topo->audio.params.size(); i++)
    if (IsTweakableParam(topo, i))
      _maxWidth = std::max(_maxWidth, 
        FBGUIGetStringWidthCached(topo->audio.params[i].displayName));
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
FBLastTweakedLabel::AudioParamChanged(int index, double /*normalized*/, bool changedFromUI)
{
  if (!changedFromUI)
    return;

  auto const* topo = _plugGUI->HostContext()->Topo();
  if (!IsTweakableParam(topo, index))
    return;

  auto const& param = topo->audio.params[index];
  auto const& module = topo->modules[param.runtimeModuleIndex];
  auto const& staticModule = topo->static_->modules[module.topoIndices.index];
  auto const& tweakName = param.displayName;
  auto const& fullName = param.shortName;
  std::string const& moduleDisplay = staticModule.matrixName.size() ? staticModule.matrixName : module.name;
  std::string modulePlusFull = moduleDisplay + " " + fullName;
  std::string modulePlusTweak = moduleDisplay + " " + tweakName;
  if(FBGUIGetStringWidthCached(modulePlusFull) <= _maxWidth)
    setText(modulePlusFull, dontSendNotification);
  else if (FBGUIGetStringWidthCached(fullName) <= _maxWidth)
    setText(fullName, dontSendNotification);
  else if (FBGUIGetStringWidthCached(modulePlusTweak) <= _maxWidth)
    setText(modulePlusTweak, dontSendNotification);
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
  addListener(this);
  setFont(FBGUIGetFont());
  setIndents(2, 8);
  setColour(TextEditor::textColourId, Colours::green);
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
FBLastTweakedTextBox::AudioParamChanged(int index, double normalized, bool changedFromUI)
{
  if (!changedFromUI)
    return;
  if (index < 0 || !IsTweakableParam(_plugGUI->HostContext()->Topo(), index))
    return;
  _paramIndex = index;
  auto const& param = _plugGUI->HostContext()->Topo()->audio.params[index];
  setText(param.NormalizedToText(false, normalized));
}

void
FBLastTweakedTextBox::textEditorFocusLost(TextEditor&)
{
  if (_paramIndex == -1)
    return;
  auto const& param = _plugGUI->HostContext()->Topo()->audio.params[_paramIndex];
  setText(param.NormalizedToText(false, _plugGUI->HostContext()->GetAudioParamNormalized(_paramIndex)));
}

void 
FBLastTweakedTextBox::textEditorTextChanged(TextEditor&)
{
  if (_paramIndex == -1)
    return;
  auto const& param = _plugGUI->HostContext()->Topo()->audio.params[_paramIndex];
  auto normalized = param.TextToNormalized(false, getText().toStdString());
  if (normalized.has_value())
    _plugGUI->HostContext()->PerformImmediateAudioParamEdit(_paramIndex, normalized.value());
}