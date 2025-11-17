#include <firefly_base/gui/components/FBMSEGEditor.hpp>

using namespace juce;

FBMSEGEditor::
FBMSEGEditor(
  int maxPoints,
  int maxLengthRatioNum,
  int maxLengthRatioDen,
  double maxLengthReal,
  int gridMinRatioGranularity):
Component(),
_maxPoints(maxPoints),
_maxLengthRatioNum(maxLengthRatioNum),
_maxLengthRatioDen(maxLengthRatioDen),
_maxLengthReal(maxLengthReal),
_gridMinRatioGranularity(gridMinRatioGranularity) {}

void
FBMSEGEditor::ChangeModel(FBMSEGModel const& model)
{
  _model = model;
  repaint();
}

void 
FBMSEGEditor::paint(Graphics& g)
{
  g.fillAll(Colours::yellow);
}