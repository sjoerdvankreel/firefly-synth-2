#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>

#include <string>
#include <optional>

struct IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBParamNonRealTime);
  virtual bool IsList() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;
  virtual float PlainToNormalized(int plain) const = 0;
  virtual double NormalizedToPlain(float normalized) const = 0; // TODO pffft
  virtual std::string PlainToText(FBValueTextDisplay display, int plain) const = 0;
  virtual std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const = 0;
};