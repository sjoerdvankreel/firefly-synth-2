#include <firefly_base/gui/components/FBMSEGEditor.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

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
  int const outerPad = 1;
  int const innerPad = 3;
  float const pointRadius = 4.0f;
  auto const outerBounds = getLocalBounds().reduced(outerPad);
  auto const innerBounds = outerBounds.reduced(innerPad);

  g.fillAll(Colours::black);
  g.setColour(Colour(0xFF181818));
  g.fillRoundedRectangle(outerBounds.toFloat(), 2.0f * innerPad);

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
  double w = innerBounds.getWidth();
  double h = innerBounds.getHeight();

  Path path = {};
  _activePointCount = 0;
  _currentPointsScreenX.clear();
  _currentPointsScreenY.clear();
  float zeroPointScreenY = (float)(h + innerPad + outerPad);
  for (int i = 0; i < _model.points.size(); i++)
  {
    bool anyPointsAfterThis = false;
    for (int j = i; j < _model.points.size(); j++)
      anyPointsAfterThis |= _currentSegmentLengths[j] > 0.0;
    if (!anyPointsAfterThis)
      break;

    double currentYNorm = _model.points[i].y;
    double currentXNorm = prevXNorm + _currentSegmentLengths[i] / totalLength;

    float prevXScreen = (float)(prevXNorm * w + innerPad + outerPad);
    float currentXScreen = (float)(currentXNorm * w + innerPad + outerPad);
    float prevYScreen = (float)((1.0f - prevYNorm) * h + innerPad + outerPad);
    float currentYScreen = (float)((1.0f - currentYNorm) * h + innerPad + outerPad);

    if (i == 0)
    {
      _initPointScreenX = prevXScreen;
      _initPointScreenY = prevYScreen;
      path.startNewSubPath(_initPointScreenX, zeroPointScreenY);
      path.lineTo(_initPointScreenX, _initPointScreenY);
    }

    // Ok so can't use beziers - they divert too much from what audio
    // is actually doing at the extremes. So, pixel by pixel it is.
    double currentSlopeNorm = _model.points[i].slope;
    int steps = (int)std::round(currentXScreen - prevXScreen);
    for (int j = 0; j < steps; j++)
    {
      // Keep in check with the actual envelope generator (ugly, i know).
      float stepOffset = (float)j / (float)steps;
      float slope = FBEnvMinSlope + (float)currentSlopeNorm * FBEnvSlopeRange;
      float stepXScreen = prevXScreen + (currentXScreen - prevXScreen) * stepOffset;
      float stepYScreen = prevYScreen + (currentYScreen - prevYScreen) * std::pow(stepOffset, std::log(slope) * FBInvLogHalf);
      path.lineTo(stepXScreen, stepYScreen);
    }

    _currentPointsScreenX.push_back(currentXScreen);
    _currentPointsScreenY.push_back(currentYScreen);
    prevXNorm = currentXNorm;
    prevYNorm = currentYNorm;
    _activePointCount++;
  }

  if (_currentPointsScreenX.size() > 0)
  {
    path.lineTo(_currentPointsScreenX[_currentPointsScreenX.size() - 1], zeroPointScreenY);
    path.closeSubPath();
    g.setColour(Colours::grey);
    g.fillPath(path);
  }

  g.setColour(Colours::white);
  g.fillEllipse(
    _initPointScreenX - pointRadius, _initPointScreenY - pointRadius, 
    2.0f * pointRadius, 2.0f * pointRadius);
  for (int i = 0; i < _activePointCount; i++)
  {
    float pointX = _currentPointsScreenX[i];
    float pointY = _currentPointsScreenY[i];
    g.fillEllipse(
      pointX - pointRadius, pointY - pointRadius,
      2.0f * pointRadius, 2.0f * pointRadius);

    float prevPointX = i == 0 ? _initPointScreenX : _currentPointsScreenX[i - 1];
    float prevPointY = i == 0 ? _initPointScreenY : _currentPointsScreenY[i - 1];
    float slopeX = prevPointX + (pointX - prevPointX) * 0.5f;
    float slopeY = prevPointY + (pointY - prevPointY) * 0.5f;
    g.drawEllipse(
      slopeX - pointRadius, slopeY - pointRadius,
      2.0f * pointRadius, 2.0f * pointRadius, 1.0f);
  }
}