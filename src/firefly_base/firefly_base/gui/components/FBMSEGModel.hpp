#pragma once

#include <vector>

enum class FBMSEGXMode
{
  Real,
  Ratio
};

enum class FBMSEGYMode
{
  Linear,
  Exponential
};

enum class FBMSEGXEditMode
{
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
  double lengthReal = {};
  int lengthRatioNum = {};
  int lengthRatioDen = {};
  
  inline double lengthRatioAsReal() const {
    return (double)lengthRatioNum / (double)lengthRatioDen;
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
