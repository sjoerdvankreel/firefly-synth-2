#pragma once

#include <playground_base/base/state/FBGUIState.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <utility>

struct FBStaticTopoGUI;

class FBPlugGUIContext
{
  float _systemScale = 1.0f;
  FBGUIState* const _state;
  FBStaticTopoGUI const* const _topo;

  float CombinedScale() const;

protected:
  FBPlugGUIContext(
    FBStaticTopoGUI const* topo,
    FBGUIState* state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUIContext);
  virtual void RequestRescale(float combinedScale) = 0;

public:
  virtual ~FBPlugGUIContext() {}

  void SetSystemScale(float scale);
  void SetUserScaleByHostWidth(int width);
  std::pair<int, int> GetHostSize() const;
  int ClampHostWidthForScale(int width) const;
  int GetHeightForAspectRatio(int width) const;

  virtual void RemoveFromDesktop() = 0;
  virtual void AddToDesktop(void* parent) = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual void SetParamNormalized(int index, float normalized) = 0;
};