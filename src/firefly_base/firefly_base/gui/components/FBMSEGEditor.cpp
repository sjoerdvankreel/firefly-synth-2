#include <firefly_base/gui/components/FBMSEGEditor.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

using namespace juce;

static float const MSEGMouseNear = 5;

FBMSEGEditor::
FBMSEGEditor(
  FBPlugGUI* plugGUI,
  int maxPoints,
  int maxLengthRatioNum,
  int maxLengthRatioDen,
  double maxLengthReal,
  int gridMinRatioGranularity):
Component(),
_plugGUI(plugGUI),
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
FBMSEGEditor::mouseExit(MouseEvent const& event)
{
  Component::mouseExit(event);
  _plugGUI->HideTooltip();
  setMouseCursor(MouseCursor::NormalCursor);
}

void
FBMSEGEditor::mouseMove(MouseEvent const& e)
{
  Component::mouseMove(e);
  int hitIndex = -1;
  auto hitType = GetNearestHit(e.position, &hitIndex);
  setMouseCursor(hitType == FBMSEGNearestHitType::None ? MouseCursor::NormalCursor : MouseCursor::DraggingHandCursor);
  if (hitType == FBMSEGNearestHitType::None)
    _plugGUI->HideTooltip();
  else if (hitType == FBMSEGNearestHitType::Init)
    _plugGUI->SetTooltip(e.getScreenPosition(), "Init");
  else if (hitType == FBMSEGNearestHitType::Point)
    _plugGUI->SetTooltip(e.getScreenPosition(), std::to_string(hitIndex + 1));
  else if (hitType == FBMSEGNearestHitType::Slope)
    _plugGUI->SetTooltip(e.getScreenPosition(), std::to_string(hitIndex + 1));
  else
    FB_ASSERT(false);
}

FBMSEGNearestHitType
FBMSEGEditor::GetNearestHit(juce::Point<float> const& p, int* index)
{
  int minPointDistanceIndex = -1;
  int minSlopeDistanceIndex = -1;
  float initDistance = p.getDistanceFrom(_initPointScreen);
  float minSlopeDistance = std::numeric_limits<float>::infinity();
  float minPointDistance = std::numeric_limits<float>::infinity();
  for (int i = 0; i < _activePointCount; i++)
  {
    auto pointDistance = p.getDistanceFrom(_currentPointsScreen[i]);
    if(pointDistance < minPointDistance)
    {
      minPointDistance = pointDistance;
      minPointDistanceIndex = i;
    }
    auto slopeDistance = p.getDistanceFrom(_currentSlopesScreen[i]);
    if (slopeDistance < minSlopeDistance)
    {
      minSlopeDistance = slopeDistance;
      minSlopeDistanceIndex = i;
    }
  }
  
  *index = -1;
  if (initDistance < minSlopeDistance && initDistance < minPointDistance)
    return initDistance <= MSEGMouseNear ? FBMSEGNearestHitType::Init : FBMSEGNearestHitType::None;
  if (minSlopeDistance < minPointDistance)
  {
    if (minSlopeDistance <= MSEGMouseNear)
    {
      *index = minSlopeDistanceIndex;
      return FBMSEGNearestHitType::Slope;
    }
    return FBMSEGNearestHitType::None;
  }
  if (minPointDistance <= MSEGMouseNear)
  {
    *index = minPointDistanceIndex;
    return FBMSEGNearestHitType::Point;
  }
  return FBMSEGNearestHitType::None;
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
  _currentPointsScreen.clear();
  _currentSlopesScreen.clear();
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
      _initPointScreen.setX(prevXScreen);
      _initPointScreen.setY(prevYScreen);
      path.startNewSubPath(_initPointScreen.getX(), zeroPointScreenY);
      path.lineTo(_initPointScreen.getX(), _initPointScreen.getY());
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

    _currentPointsScreen.push_back({ currentXScreen, currentYScreen });
    prevXNorm = currentXNorm;
    prevYNorm = currentYNorm;
    _activePointCount++;
  }

  if (_currentPointsScreen.size() > 0)
  {
    path.lineTo(_currentPointsScreen[_currentPointsScreen.size() - 1].getX(), zeroPointScreenY);
    path.closeSubPath();
    g.setColour(Colours::grey);
    g.fillPath(path);
  }

  g.setColour(Colours::white);
  g.fillEllipse(
    _initPointScreen.getX() - pointRadius, _initPointScreen.getY() - pointRadius,
    2.0f * pointRadius, 2.0f * pointRadius);
  for (int i = 0; i < _activePointCount; i++)
  {
    float pointX = _currentPointsScreen[i].getX();
    float pointY = _currentPointsScreen[i].getY();
    g.fillEllipse(
      pointX - pointRadius, pointY - pointRadius,
      2.0f * pointRadius, 2.0f * pointRadius);

    float slope = FBEnvMinSlope + (float)_model.points[i].slope * FBEnvSlopeRange;
    float prevPointX = i == 0 ? _initPointScreen.getX() : _currentPointsScreen[i - 1].getX();
    float prevPointY = i == 0 ? _initPointScreen.getY() : _currentPointsScreen[i - 1].getY();
    float slopeX = prevPointX + (pointX - prevPointX) * 0.5f;
    float slopeY = prevPointY + (pointY - prevPointY) * std::pow(0.5f, std::log(slope) * FBInvLogHalf);
    g.drawEllipse(
      slopeX - pointRadius, slopeY - pointRadius,
      2.0f * pointRadius, 2.0f * pointRadius, 1.0f);
    _currentSlopesScreen.push_back({ slopeX, slopeY });
  }
}