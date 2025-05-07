#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <utility>

class FBHostGUIContext;
struct FBStaticGUIParam;
struct FBSpecialGUIParam;

class FBPlugGUIContext
{
  double _systemScale = 1.0;
  FBHostGUIContext* const _hostContext;
  double CombinedScale() const;

  FBStaticGUIParam const& UserScaleParam() const;
  FBSpecialGUIParam const& UserScaleSpecial() const;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContext);
  FBPlugGUIContext(FBHostGUIContext* const hostContext);
  virtual void RequestRescale(double combinedScale) = 0;

public:
  void SetSystemScale(double scale);
  void SetUserScaleByHostWidth(int width);
  std::pair<int, int> GetHostSize() const;
  int ClampHostWidthForScale(int width) const;
  int GetHeightForAspectRatio(int width) const;
  FBHostGUIContext* const HostContext() const { return _hostContext; }

  virtual ~FBPlugGUIContext() {}
  virtual void RemoveFromDesktop() = 0;
  virtual void UpdateExchangeState() = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual void AddToDesktop(void* parent) = 0;
  virtual void SetAudioParamNormalizedFromHost(int index, double normalized) = 0;
};