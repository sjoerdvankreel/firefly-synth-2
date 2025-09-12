#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGraph.hpp>

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

using namespace juce;

FFModMatrixGraph::
~FFModMatrixGraph()
{
  _plugGUI->RemoveParamListener(this);
}

FFModMatrixGraph::
FFModMatrixGraph(FFPlugGUI* plugGUI, FFModMatrixGraphType type) :
_plugGUI(plugGUI), _type(type)
{
  _plugGUI->AddParamListener(this);
}

void 
FFModMatrixGraph::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const* param = &_plugGUI->HostContext()->Topo()->audio.params[index];
  if (param->topoIndices.module.index == (int)FFModuleType::VMatrix &&
    param->static_.slotCount == FFModMatrixVoiceMaxSlotCount)
  {
    _trackingParam = param;
    repaint();
  }
  if (param->topoIndices.module.index == (int)FFModuleType::GMatrix &&
    param->static_.slotCount == FFModMatrixGlobalMaxSlotCount)
  {
    _trackingParam = param;
    repaint();
  }
}

void
FFModMatrixGraph::paint(Graphics& g)
{
  float sourceRange = 1.0f;
  float sourceOffset = 0.0f;
  if (_trackingParam != nullptr)
  {
    int slot = _trackingParam->topoIndices.param.slot;
    FBTopoIndices module = _trackingParam->topoIndices.module;
    sourceRange = (float)_plugGUI->HostContext()->GetAudioParamIdentity({ module, { (int)FFModMatrixParam::SourceRange, slot } });
    sourceOffset = (float)_plugGUI->HostContext()->GetAudioParamIdentity({ module, { (int)FFModMatrixParam::SourceOffset, slot } });
  }

  std::string text = {};
  std::vector<float> yNormalized = {};
  auto bounds = getBounds().toFloat().reduced(2.0f);
  switch (_type)
  {
  case FFModMatrixGraphType::Source:
    text = "Source";
    for (int i = 0; i < bounds.getWidth(); i++)
      yNormalized.push_back(FBToUnipolar(std::sin(2.0f * FBPi * i / (float)bounds.getWidth())));
    break;
  case FFModMatrixGraphType::SourceLowHigh:
    text = "Low/High"; // TODO use the actual algos (not data) from dsp
    for (int i = 0; i < bounds.getWidth(); i++)
    {
      float sourceNorm = FBToUnipolar(std::sin(2.0f * FBPi * i / (float)bounds.getWidth()));
      sourceNorm = std::clamp(sourceNorm, sourceOffset, sourceOffset + (1.0f - sourceOffset) * sourceRange);
      yNormalized.push_back(sourceNorm);
    }
    break;
  default:
    yNormalized.push_back(0.5);
    yNormalized.push_back(0.5);
    break;
  }

  g.setColour(Colours::darkgrey);
  g.setFont(FBGUIGetFont().withHeight(16.0f));
  g.drawText(text, bounds, Justification::centred, false);

  Path path;
  path.startNewSubPath(bounds.getX(), bounds.getY() + bounds.getHeight() * (1.0f - yNormalized[0]));
  for (int i = 1; i < yNormalized.size(); i++)
    path.lineTo(bounds.getX() + i, bounds.getY() + bounds.getHeight() * (1.0f - yNormalized[i]));
  g.setColour(Colours::white);
  g.strokePath(path, PathStrokeType(1.0f));  
}