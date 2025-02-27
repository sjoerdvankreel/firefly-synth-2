#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>

#include <string>
#include <optional>

struct IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBParamNonRealTime);
  virtual int ValueCount() const = 0;
  virtual float PlainToNormalized(int plain) const = 0;
  virtual int NormalizedToPlain(float normalized) const = 0;
  virtual std::string PlainToText(FBValueTextDisplay display, int plain) const = 0;
  virtual std::optional<int> TextToPlain(bool io, std::string const& text) const = 0;
};