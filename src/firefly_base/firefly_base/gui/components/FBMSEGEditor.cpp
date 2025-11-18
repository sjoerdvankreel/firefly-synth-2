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
    double currentSlopeNorm = _model.points[i].slope;

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
    
    float currentExpoControlXScreen = prevXScreen + (currentXScreen - prevXScreen) * (1.0f - (float)currentSlopeNorm);
    float currentExpoControlYScreen = prevYScreen + (currentYScreen - prevYScreen) * (float)currentSlopeNorm;
    path.quadraticTo(currentExpoControlXScreen, currentExpoControlYScreen, currentXScreen, currentYScreen);

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
    g.fillEllipse(
      _currentPointsScreenX[i] - pointRadius, _currentPointsScreenY[i] - pointRadius,
      2.0f * pointRadius, 2.0f * pointRadius);
}