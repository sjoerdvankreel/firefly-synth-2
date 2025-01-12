#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class IFBPlugGUI
{
protected:
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBPlugGUI);

public:
  virtual ~IFBPlugGUI() {}

  virtual int GetAspectRatioWidth() const = 0;
  virtual int GetAspectRatioHeight() const = 0;
  virtual int GetMinUnscaledWidth() const = 0;
  virtual int GetMaxUnscaledWidth() const = 0;
  virtual int GetDefaultUnscaledWidth() const = 0;
  virtual int GetDefaultUnscaledHeight() const = 0;

  virtual int GetScaledWidth() const = 0;
  virtual int GetScaledHeight() const = 0;
  virtual int GetMinScaledWidth() const = 0;
  virtual int GetMaxScaledWidth() const = 0;
  virtual void SetScaledSize(int w, int h) = 0;
  virtual void SetContentScaleFactor(float scale) = 0;

  virtual void RemoveFromDesktop() = 0;
  virtual void AddToDesktop(void* parent) = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual void SetParamNormalized(int index, float normalized) = 0;
};