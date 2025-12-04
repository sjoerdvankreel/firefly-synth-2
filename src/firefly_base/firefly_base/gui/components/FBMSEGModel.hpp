#pragma once

#include <firefly_base/base/topo/static/FBBarsItem.hpp>
#include <vector>

enum class FBMSEGXMode
{
  Time,
  Bars
};

enum class FBMSEGYMode
{
  Linear,
  Exponential
};

enum class FBMSEGXEditMode
{
  Off,
  Free,
  Snap,
  Stretch,
  Count
};

enum class FBMSEGYEditMode
{
  Free,
  Snap,
  Count
};

enum class FBMSEGNearestHitType
{
  None,
  Start,
  Point,
  Slope,
};

struct FBMSEGPoint
{
  double y = {};
  double slope = {};
  double lengthTime = {};
  FBBarsItem lengthBars = {};
  
  inline double lengthBarsAsReal() const {
    return (double)lengthBars.num / (double)lengthBars.denom;
  }
};

struct FBMSEGModel
{
  bool enabled = {};
  bool looping = {};
  bool releasing = {};
  double startY = {};
  int loopStart = {};
  int loopLength = {};
  int releasePoint = {};
  int snapXCount = 16;
  int snapYCount = 16;

  FBMSEGXMode xMode = {};
  FBMSEGYMode yMode = {};
  FBMSEGXEditMode xEditMode = {};
  FBMSEGYEditMode yEditMode = {};
  std::vector<FBMSEGPoint> points = {};
};
