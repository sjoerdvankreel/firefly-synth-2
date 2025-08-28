#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <optional>

struct FBStaticParamBase;
enum class FBEditType { Linear, Stepped, Logarithmic };
enum class FBParamType { List, Bars, Boolean, Discrete, Log2, Linear, Identity };

std::string
FBEditTypeToString(FBEditType type);

struct FBParamNonRealTime
{
  virtual bool IsItems() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;
  virtual int ValueOffset() const = 0;
  virtual FBEditType GUIEditType() const = 0;
  virtual FBEditType AutomationEditType() const = 0;

  virtual double PlainToNormalized(double plain) const = 0;
  virtual double NormalizedToPlain(double normalized) const = 0;
  virtual std::string PlainToText(bool io, int moduleIndex, double plain) const = 0;
  virtual std::optional<double> TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const = 0;

  std::string NormalizedToText(bool io, int moduleIndex, double normalized) const;
  std::optional<double> TextToPlain(FBStaticParamBase const& param, bool io, int moduleIndex, std::string const& text) const;
  std::optional<double> TextToNormalized(FBStaticParamBase const& param, bool io, int moduleIndex, std::string const& text) const;
};

struct FBItemsParamNonRealTime :
public FBParamNonRealTime
{
  virtual juce::PopupMenu MakePopupMenu(int moduleIndex) const = 0;
};