#include <firefly_base/gui/components/FBMSEGEditor.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

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
FBMSEGEditor::UpdateModel()
{
  FB_ASSERT(_model.gridEditRatioGranularity > 0);
  FB_ASSERT(0.0 <= _model.initialY && _model.initialY <= 1.0);
  FB_ASSERT(_model.loopLength <= _maxPoints);
  FB_ASSERT(0 <= _model.loopStart && _model.loopStart < _maxPoints);
  FB_ASSERT(_model.points.size() <= _maxPoints);
  FB_ASSERT(0 <= _model.releasePoint && _model.releasePoint < _maxPoints);
  repaint();
}

void 
FBMSEGEditor::paint(Graphics& g)
{
  g.fillAll(Colours::black);
  double totalLength = 0.0;
  _currentSegmentLengths.clear();
  for (int i = 0; i < _model.points.size(); i++)
  {
    double currentSegmentLength;
    if (_model.xMode == FBMSEGXMode::Real)
      currentSegmentLength = _model.points[i].lengthReal;
    else
      currentSegmentLength = _model.points[i].lengthRatioAsReal();
    _currentSegmentLengths.push_back(currentSegmentLength);
    totalLength += currentSegmentLength;
  }

  double prevXNorm = 0.0;
  double prevYNorm = _model.initialY;
  double w = getBounds().getWidth();
  double h = getBounds().getHeight();

  for (int i = 0; i < _model.points.size(); i++)
  {
    double currentYNorm = _model.points[i].y;
    double currentXNorm = prevXNorm + _currentSegmentLengths[i] / totalLength;

    float prevX = (float)(prevXNorm * w);
    float currentX = (float)(currentXNorm * w);
    float prevY = (float)(prevYNorm * h);
    float currentY = (float)(currentYNorm * h);

    g.setColour(Colours::yellow);
    g.drawLine(prevX, prevY, currentX, currentY, 1.0f);

    prevXNorm = currentXNorm;
    prevYNorm = currentYNorm;
  }
}