#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>

static void
DropOutliers(std::vector<float>& v, float maxStDevTimes)
{
  if (v.size() == 0)
    return;
  float sum = 0.0f;
  for (int i = 0; i < v.size(); i++)
    sum += v[i];
  float mean = sum / (float)v.size();

  float variance = 0.0f;
  for (int i = 0; i < v.size(); i++)
  {
    float devSqr = (v[i] - mean) * (v[i] - mean);
    variance += devSqr;
  }
  variance /= (float)v.size();
  float stDev = std::sqrt(variance);

  for (int i = 0; i < v.size(); i++)
  {
    if (v[i] < mean - maxStDevTimes * stDev)
      v[i] = mean - maxStDevTimes * stDev;
    if (v[i] > mean + maxStDevTimes * stDev)
      v[i] = mean + maxStDevTimes * stDev;
  }
}

void 
FBModuleGraphComponentData::DropOutliers(float maxStDevTimes)
{
  for (int i = 0; i < graphs.size(); i++)
    graphs[i].DropOutliers(maxStDevTimes);
}

void 
FBModuleGraphComponentData::ScaleToAllNormalized()
{
  float factor = 1.0f;
  if (GetScaleFactorToAllNormalized(factor))
    ScaleBy(factor);
}

void 
FBModuleGraphComponentData::ScaleBy(float factor)
{
  for (int i = 0; i < graphs.size(); i++)
    graphs[i].ScaleBy(factor);
}

bool 
FBModuleGraphComponentData::GetScaleFactorToAllNormalized(float& factor) const
{
  bool any = false;
  float min = std::numeric_limits<float>::max();
  for (int i = 0; i < graphs.size(); i++)
  {
    float thisFactor = 0.0f;
    if (graphs[i].GetScaleFactorToNormalized(thisFactor))
    {
      any = true;
      min = std::min(min, thisFactor);
    }
  }
  if (any)
  {
    factor = min;
    return true;
  }
  factor = 1.0f;
  return false;
}

void 
FBModuleGraphData::DropOutliers(float maxStDevTimes)
{
  ::DropOutliers(primarySeries.l, maxStDevTimes);
  ::DropOutliers(primarySeries.r, maxStDevTimes);
  for (int i = 0; i < secondarySeries.size(); i++)
  {
    ::DropOutliers(secondarySeries[i].points.l, maxStDevTimes);
    ::DropOutliers(secondarySeries[i].points.r, maxStDevTimes);
  }
}

void
FBModuleGraphData::MergeStereo()
{
  for (int i = 0; i < primarySeries.l.size(); i++)
    primarySeries.l[i] = (primarySeries.l[i] + primarySeries.r[i]) * 0.5f;
  primarySeries.r.clear();
  for (int s = 0; s < secondarySeries.size(); s++)
  {
    for (int i = 0; i < secondarySeries[s].points.l.size(); i++)
      secondarySeries[s].points.l[i] = (secondarySeries[s].points.l[i] + secondarySeries[s].points.r[i]) * 0.5f;
    secondarySeries[s].points.r.clear();
  }
}

bool
FBModuleGraphData::GetScaleFactorToNormalized(float& factor) const
{
  float max = 0.0f;
  for (int i = 0; i < primarySeries.l.size(); i++)
    max = std::max(max, std::abs(primarySeries.l[i]));
  for (int i = 0; i < primarySeries.r.size(); i++)
    max = std::max(max, std::abs(primarySeries.r[i]));
  for (int s = 0; s < secondarySeries.size(); s++)
  {
    for (int i = 0; i < secondarySeries[s].points.l.size(); i++)
      max = std::max(max, std::abs(secondarySeries[s].points.l[i]));
    for (int i = 0; i < secondarySeries[s].points.r.size(); i++)
      max = std::max(max, std::abs(secondarySeries[s].points.r[i]));
  }

  if (max > 1e-6)
  {
    factor = 1.0f / max;
    FB_ASSERT(!std::isinf(factor));
    return true;
  }
  factor = 1.0f;
  return false;
}

void
FBModuleGraphData::ScaleToSelfNormalized()
{
  float factor = 1.0f;
  if (GetScaleFactorToNormalized(factor))
    ScaleBy(factor);
}

void
FBModuleGraphData::ScaleBy(float factor)
{
  for (int i = 0; i < primarySeries.l.size(); i++)
    primarySeries.l[i] *= factor;
  for (int i = 0; i < primarySeries.r.size(); i++)
    primarySeries.r[i] *= factor;
  for (int s = 0; s < secondarySeries.size(); s++)
  {
    for (int i = 0; i < secondarySeries[s].points.l.size(); i++)
      secondarySeries[s].points.l[i] *= factor;
    for (int i = 0; i < secondarySeries[s].points.r.size(); i++)
      secondarySeries[s].points.r[i] *= factor;
  }
}

void 
FBModuleGraphData::GetLimits(bool includePrimary, int& maxSizeAllSeriesOut, float& absMaxValueAllSeriesOut) const
{
  if (_maxSizeAllSeries != -1)
  {
    maxSizeAllSeriesOut = _maxSizeAllSeries;
    absMaxValueAllSeriesOut = includePrimary? _absMaxValueAllSeriesAll: _absMaxValueAllSeriesSecondary;
    return;
  }

  bool stereo = !primarySeries.r.empty();
  _absMaxValueAllSeriesAll = 0.0f;
  _absMaxValueAllSeriesSecondary = 0.0f;
  _maxSizeAllSeries = static_cast<int>(primarySeries.l.size());
  for (int i = 0; i < primarySeries.l.size(); i++)
  {
    _absMaxValueAllSeriesAll = std::max(_absMaxValueAllSeriesAll, std::abs(primarySeries.l[i]));
    if (stereo)
      _absMaxValueAllSeriesAll = std::max(_absMaxValueAllSeriesAll, std::abs(primarySeries.r[i]));
  }
  for (int i = 0; i < secondarySeries.size(); i++)
  {
    _maxSizeAllSeries = std::max(_maxSizeAllSeries, static_cast<int>(secondarySeries[i].points.l.size()));
    for (int j = 0; j < secondarySeries[i].points.l.size(); j++)
    {
      _absMaxValueAllSeriesAll = std::max(_absMaxValueAllSeriesAll, std::abs(secondarySeries[i].points.l[j]));
      _absMaxValueAllSeriesSecondary = std::max(_absMaxValueAllSeriesSecondary, std::abs(secondarySeries[i].points.l[j]));
      if (stereo)
      {
        _absMaxValueAllSeriesAll = std::max(_absMaxValueAllSeriesAll, std::abs(secondarySeries[i].points.r[j]));
        _absMaxValueAllSeriesSecondary = std::max(_absMaxValueAllSeriesSecondary, std::abs(secondarySeries[i].points.r[j]));
      }
    }
  }
  maxSizeAllSeriesOut = _maxSizeAllSeries;
  absMaxValueAllSeriesOut = includePrimary ? _absMaxValueAllSeriesAll : _absMaxValueAllSeriesSecondary;
  FB_ASSERT(!std::isinf(absMaxValueAllSeriesOut));
}