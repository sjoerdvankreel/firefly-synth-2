#include <firefly_base/gui/components/FBMSEGEditor.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

using namespace juce;

static float const MSEGMouseNear = 5;
static int const MSEGOuterPadding = 1;
static int const MSEGInnerPadding = 3;

FBMSEGEditor::
FBMSEGEditor(
  FBPlugGUI* plugGUI,
  std::string const& name,
  int maxPoints,
  int maxLengthRatioNum,
  int maxLengthRatioDen,
  double maxLengthReal,
  int gridMinRatioGranularity):
Component(),
_plugGUI(plugGUI),
_name(name),
_maxPoints(maxPoints),
_maxLengthRatioNum(maxLengthRatioNum),
_maxLengthRatioDen(maxLengthRatioDen),
_maxLengthReal(maxLengthReal),
_gridMinRatioGranularity(gridMinRatioGranularity) {}

void
FBMSEGEditor::UpdateModel()
{
  FB_ASSERT(_model.gridEditRatioGranularity > 0);
  FB_ASSERT(0.0 <= _model.startY && _model.startY <= 1.0);
  FB_ASSERT(_model.loopLength <= _maxPoints);
  FB_ASSERT(0 <= _model.loopStart && _model.loopStart <= _maxPoints);
  FB_ASSERT(_model.points.size() <= _maxPoints);
  FB_ASSERT(0 <= _model.releasePoint && _model.releasePoint <= _maxPoints);
  repaint();
}

void
FBMSEGEditor::StopDrag()
{
  _dragType = {};
  _dragIndex = -1;
  _dragging = false;
  setMouseCursor(MouseCursor::NormalCursor);
}

void
FBMSEGEditor::mouseUp(MouseEvent const& event)
{
  Component::mouseUp(event);
  StopDrag();
}

void 
FBMSEGEditor::mouseExit(MouseEvent const& event)
{
  Component::mouseExit(event);
  _plugGUI->HideTooltip();
  setMouseCursor(MouseCursor::NormalCursor);
  StopDrag();
}

void
FBMSEGEditor::mouseDown(MouseEvent const& event)
{
  Component::mouseDown(event);
  int hitIndex = -1;
  auto hitType = GetNearestHit(event.position, &hitIndex);
  if (!_model.enabled || hitType == FBMSEGNearestHitType::None || _currentPointsScreen.size() == 0)
  {
    StopDrag();
    return;
  }
  _dragging = true;
  _dragType = hitType;
  _dragIndex = hitIndex;
  _plugGUI->HostContext()->UndoState().Snapshot("Change " + _name + " MSEG");
}

void
FBMSEGEditor::mouseMove(MouseEvent const& e)
{
  Component::mouseMove(e);
  if (_currentPointsScreen.size() == 0)
  {
    _plugGUI->SetTooltip(e.getScreenPosition(), "Double-click to add a point.");
    return;
  }

  int hitIndex = -1;
  auto hitType = GetNearestHit(e.position, &hitIndex);
  setMouseCursor((!_model.enabled || hitType == FBMSEGNearestHitType::None) ? MouseCursor::NormalCursor : MouseCursor::DraggingHandCursor);
  if (hitType == FBMSEGNearestHitType::None)
    _plugGUI->HideTooltip();
  else if (getTooltipFor != nullptr)
    _plugGUI->SetTooltip(e.getScreenPosition(), getTooltipFor(hitType, hitIndex));
}

void
FBMSEGEditor::mouseDoubleClick(MouseEvent const& event)
{
  auto const outerBounds = getLocalBounds().reduced(MSEGOuterPadding);
  auto const innerBounds = outerBounds.reduced(MSEGInnerPadding);

  if (_currentPointsScreen.size() == 0)
  {
    _model.points[0].lengthReal = _maxLengthReal * 0.5f;
    if (modelUpdated != nullptr)
    {
      _plugGUI->HostContext()->UndoState().Snapshot("Change " + _name + " MSEG");
      modelUpdated(_model);
    }
    return;
  }

  int hitIndex = -1;
  auto hitType = GetNearestHit(event.position, &hitIndex);
  if (hitType == FBMSEGNearestHitType::Start)
    _model.startY = 0.0f;
  else if (hitType == FBMSEGNearestHitType::Slope)
    _model.points[hitIndex].slope = 0.5f;
  else if (hitType == FBMSEGNearestHitType::Point)
  {
    // delete a point, adjust loop and release

    for (int i = hitIndex; i < _model.points.size() - 1; i++)
      _model.points[i] = _model.points[i + 1];
    _model.points[_model.points.size() - 1] = {};

    if (_model.releasing)
    {
      int hitIndexRelease = hitIndex;
      if (hitIndexRelease <= _model.releasePoint)
      {
        if (_model.releasePoint > 0)
          _model.releasePoint--;
        else
          _model.releasing = false;
      }
    }

    if (_model.looping)
    {
      int hitIndexLoop = hitIndex + 1;
      if (hitIndexLoop <= _model.loopStart)
      {
        if (_model.loopStart > 0)
          _model.loopStart--;
        else
          _model.looping = false;
      }
      else if (hitIndexLoop <= _model.loopStart + _model.loopLength)
      {
        if (_model.loopLength > 0)
          _model.loopLength--;
      }
    }
  }
  else
  {
    // split a segment, if allowed
    // adjust loop and release

    FB_ASSERT(hitType == FBMSEGNearestHitType::None);
    if (_currentPointsScreen.size() < _model.points.size())
    {
      for (int i = 0; i < _currentPointsScreen.size(); i++)
      {
        float prevX = i == 0 ? _startPointScreen.getX() : _currentPointsScreen[i - 1].getX();
        float thisX = _currentPointsScreen[i].getX();
        float clickYNorm = std::clamp(1.0f - (event.position.y - innerBounds.getY()) / innerBounds.getHeight(), 0.0f, 1.0f);
        if (prevX <= event.position.x && event.position.x <= thisX)
        {
          float lengthToSplit = (float)_model.points[i].lengthReal;
          float splitPosNorm = std::clamp((event.position.x - prevX) / (thisX - prevX), 0.0f, 1.0f);
          for (int j = _maxPoints - 1; j > i; j--)
            _model.points[j] = _model.points[j - 1];
          _model.points[i].y = clickYNorm;
          _model.points[i].lengthReal = lengthToSplit * splitPosNorm;
          _model.points[i + 1].lengthReal = lengthToSplit * (1.0f - splitPosNorm);

          if (_model.releasing)
          {
            if (i <= _model.releasePoint)
              if (_model.releasePoint < _currentPointsScreen.size())
                _model.releasePoint++;
          }

          break;
        }
      }
    }
  }

  if (modelUpdated != nullptr)
  {
    _plugGUI->HostContext()->UndoState().Snapshot("Change " + _name + " MSEG");
    modelUpdated(_model);
  }
}

void 
FBMSEGEditor::mouseDrag(MouseEvent const& event)
{
  Component::mouseDrag(event);
  if (!_dragging)
    return;

  if (_currentPointsScreen.size() == 0)
  {
    StopDrag();
    return;
  }

  auto const outerBounds = getLocalBounds().reduced(MSEGOuterPadding);
  auto const innerBounds = outerBounds.reduced(MSEGInnerPadding);
  auto adjustedPosition = event.position;
  if (adjustedPosition.x < innerBounds.getX())
    adjustedPosition.x = (float)innerBounds.getX();
  if (adjustedPosition.y < innerBounds.getY())
    adjustedPosition.y = (float)innerBounds.getY();
  if (adjustedPosition.y > innerBounds.getBottom())
    adjustedPosition.y = (float)innerBounds.getBottom();
  // Do NOT check for exceed right bounds - we need it to allow growing the last segment.

  double h = innerBounds.getHeight();
  if (_dragType == FBMSEGNearestHitType::Start)
  {
    double yNorm = std::clamp(1.0 - (adjustedPosition.y - MSEGInnerPadding - MSEGOuterPadding) / h, 0.0, 1.0);
    _model.startY = yNorm;

    if (modelUpdated != nullptr)
      modelUpdated(_model);
    return;
  }

  if (_dragIndex == -1 || _dragIndex >= (int)_currentPointsScreen.size())
  {
    // Can happen because the whole paint-drag-paint-drag cycle
    // is not in the same callstack. Better luck next round.
    return;
  }

  if (_dragType == FBMSEGNearestHitType::Slope)
  {
    double pointToY = _currentPointsScreen[_dragIndex].getY();
    double pointFromY = _dragIndex == 0? _startPointScreen.getY(): _currentPointsScreen[_dragIndex - 1].getY();
    double yNorm = std::clamp((adjustedPosition.y - pointFromY) / (pointToY - pointFromY), 0.0, 1.0);
    _model.points[_dragIndex].slope = yNorm;
    
    if (modelUpdated != nullptr)
      modelUpdated(_model);
    return;
  }

  if (_dragType == FBMSEGNearestHitType::Point)
  {
    double yNorm = std::clamp(1.0 - (adjustedPosition.y - MSEGInnerPadding - MSEGOuterPadding) / h, 0.0, 1.0);
    _model.points[_dragIndex].y = yNorm;

    double xCurrent = _currentPointsScreen[_dragIndex].getX();
    double xBefore = _dragIndex == 0 ? _startPointScreen.getX() : _currentPointsScreen[_dragIndex - 1].getX();
    double segLen = xCurrent - xBefore;
    double dragLen = adjustedPosition.x - xBefore;
    _model.points[_dragIndex].lengthReal += (dragLen - segLen) * _totalLengthReal / _totalLengthScreen;
    _model.points[_dragIndex].lengthReal = std::clamp(_model.points[_dragIndex].lengthReal, 0.0, _maxLengthReal);

    if (modelUpdated != nullptr)
      modelUpdated(_model);
    return;
  }
}

FBMSEGNearestHitType
FBMSEGEditor::GetNearestHit(juce::Point<float> const& p, int* index)
{
  if (_currentPointsScreen.size() == 0)
    return FBMSEGNearestHitType::None;

  int minPointDistanceIndex = -1;
  int minSlopeDistanceIndex = -1;
  float startDistance = p.getDistanceFrom(_startPointScreen);
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
    if (_model.yMode == FBMSEGYMode::Exponential)
    {
      auto slopeDistance = p.getDistanceFrom(_currentSlopesScreen[i]);
      if (slopeDistance < minSlopeDistance)
      {
        minSlopeDistance = slopeDistance;
        minSlopeDistanceIndex = i;
      }
    }
  }
  
  *index = -1;
  if (startDistance < minSlopeDistance && startDistance < minPointDistance)
    return startDistance <= MSEGMouseNear ? FBMSEGNearestHitType::Start : FBMSEGNearestHitType::None;
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
  float const pointRadius = 4.0f;
  auto const outerBounds = getLocalBounds().reduced(MSEGOuterPadding);
  auto const innerBounds = outerBounds.reduced(MSEGInnerPadding);

  g.fillAll(Colours::black);
  g.setColour(Colour(0xFF181818));
  g.fillRoundedRectangle(outerBounds.toFloat(), 2.0f * MSEGInnerPadding);

  _totalLengthReal = 0.0;
  _currentSegmentLengths.clear();
  for (int i = 0; i < _model.points.size(); i++)
  {
    double currentSegmentLength;
    if (_model.xMode == FBMSEGXMode::Real)
      currentSegmentLength = _model.points[i].lengthReal;
    else
      currentSegmentLength = _model.points[i].lengthRatioAsReal();
    _currentSegmentLengths.push_back(currentSegmentLength);
    _totalLengthReal += (float)currentSegmentLength;
  }

  double prevXNorm = 0.0;
  double prevYNorm = _model.startY;
  double y = innerBounds.getY();
  double w = innerBounds.getWidth();
  double h = innerBounds.getHeight();

  Path path = {};
  _activePointCount = 0;
  _totalLengthScreen = {};
  _currentPointsScreen.clear();
  _currentSlopesScreen.clear();
  float zeroPointScreenY = (float)(h + MSEGInnerPadding + MSEGOuterPadding);
  for (int i = 0; i < _model.points.size(); i++)
  {
    bool anyPointsAfterThis = false;
    for (int j = i; j < _model.points.size(); j++)
      anyPointsAfterThis |= _currentSegmentLengths[j] > 0.0;
    if (!anyPointsAfterThis)
      break;

    double currentYNorm = _model.points[i].y;
    double currentXNorm = prevXNorm + _currentSegmentLengths[i] / _totalLengthReal;

    float prevXScreen = (float)(prevXNorm * w + MSEGInnerPadding + MSEGOuterPadding);
    float currentXScreen = (float)(currentXNorm * w + MSEGInnerPadding + MSEGOuterPadding);
    float prevYScreen = (float)((1.0f - prevYNorm) * h + MSEGInnerPadding + MSEGOuterPadding);
    float currentYScreen = (float)((1.0f - currentYNorm) * h + MSEGInnerPadding + MSEGOuterPadding);
    _totalLengthScreen += currentXScreen - prevXScreen;

    if (i == 0)
    {
      _startPointScreen.setX(prevXScreen);
      _startPointScreen.setY(prevYScreen);
      path.startNewSubPath(_startPointScreen.getX(), zeroPointScreenY);
      path.lineTo(_startPointScreen.getX(), _startPointScreen.getY());
    }

    // Ok so can't use beziers - they divert too much from what audio
    // is actually doing at the extremes. So, pixel by pixel it is.
    double currentSlopeNorm = _model.yMode == FBMSEGYMode::Exponential ? _model.points[i].slope : 0.5f;
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

  if (_currentPointsScreen.size() == 0)
  {
    g.setColour(Colours::grey);
    g.setFont(FBGUIGetFont().withHeight(20.0f));
    g.drawText("OFF", innerBounds, Justification::centred, false);
    return;
  }

  path.lineTo(_currentPointsScreen[_currentPointsScreen.size() - 1].getX(), zeroPointScreenY);
  path.closeSubPath();
  g.setColour(_model.enabled ? Colours::grey : Colours::darkgrey);
  g.fillPath(path);
  
  if (_model.looping && _currentPointsScreen.size() > 0)
  {
    float dashes[2] = { 4, 2 };
    g.setColour(_model.enabled ? Colours::white : Colours::grey);
    int loopStart = std::min(_model.loopStart, (int)_currentPointsScreen.size());
    int loopEnd = std::min(_model.loopStart + _model.loopLength, (int)_currentPointsScreen.size());
    float loopStartXScreen = loopStart == 0 ? _startPointScreen.getX() : _currentPointsScreen[loopStart - 1].getX();
    float loopEndXScreen = loopEnd == 0 ? _startPointScreen.getX() : _currentPointsScreen[loopEnd - 1].getX();
    g.drawDashedLine(Line<float>(loopStartXScreen, (float)y, loopStartXScreen, (float)h), dashes, 2);
    g.drawDashedLine(Line<float>(loopEndXScreen, (float)y, loopEndXScreen, (float)h), dashes, 2);
  }

  g.setColour(_model.enabled? Colours::white: Colours::grey);
  g.fillEllipse(
    _startPointScreen.getX() - pointRadius, _startPointScreen.getY() - pointRadius,
    2.0f * pointRadius, 2.0f * pointRadius);
  for (int i = 0; i < _activePointCount; i++)
  {
    float pointX = _currentPointsScreen[i].getX();
    float pointY = _currentPointsScreen[i].getY();
    if(_model.releasing && (_model.releasePoint == i || _activePointCount - 1 == i && _model.releasePoint >= _activePointCount))
      g.drawEllipse(
        pointX - (pointRadius - 1.0f), pointY - (pointRadius - 1.0f),
        2.0f * pointRadius, 2.0f * pointRadius, 2.0f);
    else
      g.fillEllipse(
        pointX - pointRadius, pointY - pointRadius,
        2.0f * pointRadius, 2.0f * pointRadius);

    float pointSlope = _model.yMode == FBMSEGYMode::Exponential ? (float)_model.points[i].slope : 0.5f;
    float slope = FBEnvMinSlope + pointSlope * FBEnvSlopeRange;
    float prevPointX = i == 0 ? _startPointScreen.getX() : _currentPointsScreen[i - 1].getX();
    float prevPointY = i == 0 ? _startPointScreen.getY() : _currentPointsScreen[i - 1].getY();
    float slopeX = prevPointX + (pointX - prevPointX) * 0.5f;
    float slopeY = prevPointY + (pointY - prevPointY) * std::pow(0.5f, std::log(slope) * FBInvLogHalf);

    if(_model.yMode == FBMSEGYMode::Exponential)
      g.drawEllipse(
        slopeX - pointRadius, slopeY - pointRadius,
        2.0f * pointRadius, 2.0f * pointRadius, 1.0f);
    _currentSlopesScreen.push_back({ slopeX, slopeY });
  }
}