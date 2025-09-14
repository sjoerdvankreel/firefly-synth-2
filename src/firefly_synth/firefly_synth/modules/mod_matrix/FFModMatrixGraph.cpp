#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGraph.hpp>

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

using namespace juce;

static float
GraphGetSource(float x)
{
  return FBToUnipolar(std::sin(2.0f * FBPi * x));
}

static float
GraphGetSourceOffsetRange(float x, float sourceOffset, float sourceRange)
{
  float sourceMin = sourceOffset;
  float sourceMax = sourceOffset + (1.0f - sourceOffset) * sourceRange;
  float sourceNorm = GraphGetSource(x);
  return sourceMin + sourceNorm * (sourceMax - sourceMin);
}

static float
GraphGetScale(float x, int scaleBy)
{
  // TODO share
  if (scaleBy == 0)
    return 1.0f;
  return FBToUnipolar(std::sin(4.0f * FBPi * x));
}

static float
GraphGetScaleMinMax(float x, int scaleBy, float min, float max)
{
  // TODO share
  float scale = scaleBy == 0 ? 1.0f : FBToUnipolar(std::sin(4.0f * FBPi * x));
  return min + (max - min) * scale;
}

static float
GraphGetTarget(float x)
{
  return x;
}

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
  std::string prefix = "";
  int scaleType = 0;
  int sourceType = 0;
  int targetType = 0;
  float scaleMin = 0.0f;
  float scaleMax = 1.0f;
  float targetAmt = 1.0f;
  float sourceRange = 1.0f;
  float sourceOffset = 0.0f;
  FFModulationOpType opType = FFModulationOpType::UPAdd;

  if (_trackingParam != nullptr)
  {
    int slot = _trackingParam->topoIndices.param.slot;
    FBTopoIndices module = _trackingParam->topoIndices.module;
    scaleType = _plugGUI->HostContext()->GetAudioParamList<int>({ module, { (int)FFModMatrixParam::Scale, slot } });
    sourceType = _plugGUI->HostContext()->GetAudioParamList<int>({ module, { (int)FFModMatrixParam::Source, slot } });
    targetType = _plugGUI->HostContext()->GetAudioParamList<int>({ module, { (int)FFModMatrixParam::Target, slot } });
    scaleMin = (float)_plugGUI->HostContext()->GetAudioParamIdentity({ module, { (int)FFModMatrixParam::ScaleMin, slot } });
    scaleMax = (float)_plugGUI->HostContext()->GetAudioParamIdentity({ module, { (int)FFModMatrixParam::ScaleMax, slot } });
    targetAmt = (float)_plugGUI->HostContext()->GetAudioParamIdentity({ module, { (int)FFModMatrixParam::TargetAmt, slot } });
    sourceRange = (float)_plugGUI->HostContext()->GetAudioParamLinear({ module, { (int)FFModMatrixParam::SourceRange, slot } });
    sourceOffset = (float)_plugGUI->HostContext()->GetAudioParamLinear({ module, { (int)FFModMatrixParam::SourceOffset, slot } });
    opType = _plugGUI->HostContext()->GetAudioParamList<FFModulationOpType>({ module, { (int)FFModMatrixParam::OpType, slot } });
    prefix = ((module.index == (int)FFModuleType::GMatrix) ? std::string("G") : std::string("V")) + std::to_string(slot + 1) + " ";
  }

  std::string text = "Off";
  std::vector<float> yNormalized = {};
  auto bounds = getBounds().toFloat().reduced(2.0f);
  switch (_type)
  {
  case FFModMatrixGraphType::Source:
    text = "Source";
    for (int i = 0; i < bounds.getWidth(); i++)
      if(sourceType == 0 || opType == FFModulationOpType::Off)
        yNormalized.push_back(GraphGetSource(i / (float)bounds.getWidth()));
      else
        yNormalized.push_back(GraphGetSourceOffsetRange(i / (float)bounds.getWidth(), sourceOffset, sourceRange));
    break;
  case FFModMatrixGraphType::SourceOnOff:
    text = (sourceType == 0 || opType == FFModulationOpType::Off) ? "Source Off" : "Source On";
    if (sourceType != 0)
      for (int i = 0; i < bounds.getWidth(); i++)
        yNormalized.push_back(GraphGetSource(i / (float)bounds.getWidth()));
    break;
  case FFModMatrixGraphType::Scale:
    text = opType == FFModulationOpType::Off? "Scale Off": "Scale";
    for (int i = 0; i < bounds.getWidth(); i++)
      yNormalized.push_back(GraphGetScale(i / (float)bounds.getWidth(), scaleType));
    break;
  case FFModMatrixGraphType::ScaleOn:
    text = opType == FFModulationOpType::Off ? "Scale Off" : "Scale On";
    for (int i = 0; i < bounds.getWidth(); i++)
      yNormalized.push_back(GraphGetScaleMinMax(i / (float)bounds.getWidth(), scaleType, scaleMin, scaleMax));
    break;
  case FFModMatrixGraphType::Target:
    text = "Target";
    for (int i = 0; i < bounds.getWidth(); i++)
      yNormalized.push_back(GraphGetTarget(i / (float)bounds.getWidth()));
    break;
  case FFModMatrixGraphType::TargetOnOff:
    text = targetType == 0 ? "Target Off" : "Target On";
    for (int i = 0; i < bounds.getWidth(); i++)
    {
      // todo show all on/off in the graphs
      float x = i / (float)bounds.getWidth();
      float target = GraphGetTarget(x);
      float source = GraphGetSourceOffsetRange(x, sourceOffset, sourceRange);
      float scaleMinMax = GraphGetScaleMinMax(x, scaleType, scaleMin, scaleMax);
      if(opType != FFModulationOpType::Off)
        FFApplyModulation(opType, source, scaleMinMax * targetAmt, target);
      yNormalized.push_back(target);
    }
    break;
  default:
    FB_ASSERT(false);
    break;
  }

  g.setColour(Colours::darkgrey);
  g.setFont(FBGUIGetFont().withHeight(16.0f));
  g.drawText(prefix + text, bounds, Justification::centred, false);

  if (yNormalized.empty())
    return;

  Path path;
  path.startNewSubPath(bounds.getX(), bounds.getY() + bounds.getHeight() * (1.0f - yNormalized[0]));
  for (int i = 1; i < yNormalized.size(); i++)
    path.lineTo(bounds.getX() + i, bounds.getY() + bounds.getHeight() * (1.0f - yNormalized[i]));

  switch (_type)
  {
  case FFModMatrixGraphType::Scale:
  case FFModMatrixGraphType::Target:
  case FFModMatrixGraphType::Source:
    g.setColour(Colours::grey);
    break;
  case FFModMatrixGraphType::ScaleOn:
  case FFModMatrixGraphType::SourceOnOff:
  case FFModMatrixGraphType::TargetOnOff:
    g.setColour(Colours::white);
    break;
  default:
    FB_ASSERT(false);
    break;
  }
  g.strokePath(path, PathStrokeType(1.0f));  
}