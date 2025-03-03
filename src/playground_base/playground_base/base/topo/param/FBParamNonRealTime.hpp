#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>

#include <string>
#include <optional>

struct FBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBParamNonRealTime);
  std::string NormalizedToText(FBValueTextDisplay display, double normalized) const;
  std::optional<double> TextToNormalized(FBValueTextDisplay display, std::string const& text) const;

  virtual bool IsItems() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;
  virtual double PlainToNormalized(double plain) const = 0;
  virtual double NormalizedToPlain(double normalized) const = 0;
  virtual std::string PlainToText(FBValueTextDisplay display, double plain) const = 0;
  virtual std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const = 0;
};