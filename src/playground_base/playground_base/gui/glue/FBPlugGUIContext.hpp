#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <utility>

class FBHostGUIContext;

class FBPlugGUIContext
{
  float _systemScale = 1.0f;
  FBHostGUIContext* const _hostContext;
  float CombinedScale() const;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContext);
  FBPlugGUIContext(FBHostGUIContext* const hostContext);
  virtual void RequestRescale(float combinedScale) = 0;

public:
  void SetSystemScale(float scale);
  void SetUserScaleByHostWidth(int width);
  std::pair<int, int> GetHostSize() const;
  int ClampHostWidthForScale(int width) const;
  int GetHeightForAspectRatio(int width) const;

  virtual ~FBPlugGUIContext() {}
  virtual void RemoveFromDesktop() = 0;
  virtual void UpdateExchangeState() = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual void AddToDesktop(void* parent) = 0;
  virtual void SetParamNormalized(int index, float normalized) = 0;
  FBHostGUIContext* const HostContext() const { return _hostContext; }
};