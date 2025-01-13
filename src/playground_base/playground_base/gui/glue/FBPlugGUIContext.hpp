#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <utility>

struct FBStaticGUITopo;

class FBPlugGUIContext
{
  float _userScale = 1.0f;
  float _systemScale = 1.0f;
  FBStaticGUITopo const* const _topo;

  float CombinedScale() const;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContext);
  FBPlugGUIContext(FBStaticGUITopo const* topo);
  virtual void RequestRescale(float combinedScale) = 0;

public:
  virtual ~FBPlugGUIContext() {}

  void SetSystemScale(float scale);
  void SetUserScaleByHostWidth(int width);
  std::pair<int, int> GetHostSize() const;

  virtual void RemoveFromDesktop() = 0;
  virtual void AddToDesktop(void* parent) = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual void SetParamNormalized(int index, float normalized) = 0;
};