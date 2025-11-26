#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

class FBHostGUIContext;
struct FBStaticGUIParam;

class FBPlugGUIContext
{
  double _systemScale = 1.0;
  FBHostGUIContext* const _hostContext;
  double CombinedScale() const;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContext);
  FBPlugGUIContext(FBHostGUIContext* const hostContext);
  virtual void RequestRescale(double combinedScale) = 0;

public:
  void SetSystemScale(double scale);
  void SetUserScaleByHostWidth(int width);
  std::pair<int, int> GetHostSize() const;
  int GetHeightForAspectRatio(int width) const;
  void ClampHostSizeForScale(int& width, int& height) const;
  FBHostGUIContext* HostContext() { return _hostContext; }
  FBHostGUIContext const* HostContext() const { return _hostContext; }

  virtual ~FBPlugGUIContext() {}
  virtual void RemoveFromDesktop() = 0;
  virtual void UpdateExchangeState() = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual void AddToDesktop(void* parent) = 0;
  virtual void SetGUIParamNormalizedFromHost(int index, double normalized) = 0;
  virtual void SetAudioParamNormalizedFromHost(int index, double normalized) = 0;
};