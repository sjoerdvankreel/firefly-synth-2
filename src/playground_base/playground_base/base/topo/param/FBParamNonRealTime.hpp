#pragma once

#include <playground_base/base/topo/param/FBEditType.hpp>

#include <string>
#include <optional>

struct FBParamNonRealTime
{
  virtual bool IsItems() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;
  virtual FBEditType GUIEditType() const = 0;
  virtual FBEditType AutomationEditType() const = 0;

  std::string NormalizedToText(bool io, double plain) const;
  std::optional<double> TextToNormalized(bool io, std::string const& text) const;

  virtual double PlainToNormalized(double plain) const = 0;
  virtual double NormalizedToPlain(double normalized) const = 0;
  virtual std::string PlainToText(bool io, double plain) const = 0;
  virtual std::optional<double> TextToPlain(bool io, std::string const& text) const = 0;
};