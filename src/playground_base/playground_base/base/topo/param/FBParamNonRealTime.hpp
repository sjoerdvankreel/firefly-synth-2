#pragma once

#include <playground_base/base/topo/param/FBTextDisplay.hpp>

#include <string>
#include <optional>

struct FBParamNonRealTime
{
  virtual bool IsItems() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;

  virtual double PlainToNormalized(double plain) const = 0;
  virtual double NormalizedToPlain(double normalized) const = 0;
  virtual std::optional<double> TextToPlain(std::string const& text) const = 0;
  virtual std::string PlainToText(FBValueTextDisplay display, double plain) const = 0;

  std::optional<double> TextToNormalized(std::string const& text) const;
  std::string NormalizedToText(FBValueTextDisplay display, double plain) const;
};