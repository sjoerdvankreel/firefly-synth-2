#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>

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

void
FBModuleGraphData::ScaleToSelf()
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

  if (max > 1e-8)
  {
    for (int i = 0; i < primarySeries.l.size(); i++)
      primarySeries.l[i] /= max;
    for (int i = 0; i < primarySeries.r.size(); i++)
      primarySeries.r[i] /= max;
    for (int s = 0; s < secondarySeries.size(); s++)
    {
      for (int i = 0; i < secondarySeries[s].points.l.size(); i++)
        secondarySeries[s].points.l[s] /= max;
      for (int i = 0; i < secondarySeries[s].points.r.size(); i++)
        secondarySeries[s].points.r[s] /= max;
    }
  }
}

void 
FBModuleGraphData::GetLimits(int& maxSizeAllSeriesOut, float& absMaxValueAllSeriesOut) const
{
  if (_maxSizeAllSeries != -1)
  {
    maxSizeAllSeriesOut = _maxSizeAllSeries;
    absMaxValueAllSeriesOut = _absMaxValueAllSeries;
    return;
  }

  bool stereo = !primarySeries.r.empty();
  _absMaxValueAllSeries = 1.0f;
  _maxSizeAllSeries = static_cast<int>(primarySeries.l.size());
  for (int i = 0; i < primarySeries.l.size(); i++)
  {
    _absMaxValueAllSeries = std::max(_absMaxValueAllSeries, std::abs(primarySeries.l[i]));
    if (stereo)
      _absMaxValueAllSeries = std::max(_absMaxValueAllSeries, std::abs(primarySeries.r[i]));
  }
  for (int i = 0; i < secondarySeries.size(); i++)
  {
    _maxSizeAllSeries = std::max(_maxSizeAllSeries, static_cast<int>(secondarySeries[i].points.l.size()));
    for (int j = 0; j < secondarySeries[i].points.l.size(); j++)
    {
      _absMaxValueAllSeries = std::max(_absMaxValueAllSeries, std::abs(secondarySeries[i].points.l[j]));
      if (stereo)
        _absMaxValueAllSeries = std::max(_absMaxValueAllSeries, std::abs(secondarySeries[i].points.r[j]));
    }
  }
  maxSizeAllSeriesOut = _maxSizeAllSeries;
  absMaxValueAllSeriesOut = _absMaxValueAllSeries;
}