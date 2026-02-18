#include <firefly_base/gui/graph/FBModuleGraphComponentData.hpp>

float 
FBModuleGraphPoints::GetAbsMaxValue() const
{
  float result = 0;
  bool stereo = r.size() > 0;
  for (int i = 0; i < l.size(); i++)
  {
    result = std::max(result, std::abs(l[i]));
    if (stereo)
      result = std::max(result, std::abs(r[i]));
  }
  return result;
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
  max = std::max(max, primarySeries.GetAbsMaxValue());
  for (int s = 0; s < secondarySeries.size(); s++)
    max = std::max(max, secondarySeries[s].points.GetAbsMaxValue());

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
FBModuleGraphData::GetLimits(bool includePrimary, int& maxSizeAllSeries, float& absMaxValueAllSeries) const
{
  float absMaxValueAllSeriesSecondary = 0.0f;
  maxSizeAllSeries = static_cast<int>(primarySeries.l.size());
  float absMaxValueAllSeriesAll = std::max(absMaxValueAllSeriesAll, primarySeries.GetAbsMaxValue());
  for (int i = 0; i < secondarySeries.size(); i++)
  {
    float absMaxValueSecondary = secondarySeries[i].points.GetAbsMaxValue();
    absMaxValueAllSeriesAll = std::max(absMaxValueAllSeriesAll, absMaxValueSecondary);
    absMaxValueAllSeriesSecondary = std::max(absMaxValueAllSeriesSecondary, absMaxValueSecondary);
    maxSizeAllSeries = std::max(maxSizeAllSeries, static_cast<int>(secondarySeries[i].points.l.size()));
  }
  absMaxValueAllSeries = includePrimary ? absMaxValueAllSeriesAll : absMaxValueAllSeriesSecondary;
  FB_ASSERT(!std::isinf(absMaxValueAllSeries));
}