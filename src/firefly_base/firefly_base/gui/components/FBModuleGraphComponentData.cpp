#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>

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