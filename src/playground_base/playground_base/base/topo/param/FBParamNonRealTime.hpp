#pragma once

#include <playground_base/base/topo/param/FBTextDisplay.hpp>

#include <string>
#include <optional>

struct FBParamNonRealTime
{
  virtual bool IsItems() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;

  std::string NormalizedToText(FBTextDisplay display, double plain) const;
  std::optional<double> TextToNormalized(FBTextDisplay display, std::string const& text) const;

  virtual double PlainToNormalized(double plain) const = 0;
  virtual double NormalizedToPlain(double normalized) const = 0;
  virtual std::string PlainToText(FBTextDisplay display, double plain) const = 0;
  virtual std::optional<double> TextToPlain(FBTextDisplay display, std::string const& text) const = 0;
};